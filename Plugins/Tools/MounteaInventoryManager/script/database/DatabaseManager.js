export class DatabaseManager {
    constructor() {
        this.db = null;
        this.dbName = 'MounteaInventoryTemplateManagerDB';
        this.version = 3;
        this.stores = {
            templates: 'id',
            files: 'id',
            settings: 'key'
        };
    }

    async initialize() {
        return new Promise((resolve, reject) => {
            const request = indexedDB.open(this.dbName, this.version);

            request.onerror = () => reject(request.error);
            request.onsuccess = () => {
                this.db = request.result;
                resolve();
            };

            request.onupgradeneeded = (event) => {
                const db = event.target.result;

                Object.entries(this.stores).forEach(([storeName, keyPath]) => {
                    if (!db.objectStoreNames.contains(storeName)) {
                        const store = db.createObjectStore(storeName, { keyPath });
                        
                        if (storeName === 'files') {
                            store.createIndex('fileName', 'fileName', { unique: false });
                            store.createIndex('fileType', 'fileType', { unique: false });
                        }
                        
                        if (storeName === 'templates') {
                            store.createIndex('itemName', 'itemName', { unique: false });
                            store.createIndex('itemType', 'itemType', { unique: false });
                            store.createIndex('rarity', 'rarity', { unique: false });
                        }
                    }
                });
            };
        });
    }

    async save(storeName, data) {
        return new Promise((resolve, reject) => {
            const transaction = this.db.transaction([storeName], 'readwrite');
            const store = transaction.objectStore(storeName);
            const request = store.put(data);
            
            request.onsuccess = () => resolve(data);
            request.onerror = () => reject(request.error);
        });
    }

    async getAll(storeName) {
        return new Promise((resolve, reject) => {
            const transaction = this.db.transaction([storeName], 'readonly');
            const store = transaction.objectStore(storeName);
            const request = store.getAll();
            
            request.onsuccess = () => resolve(request.result || []);
            request.onerror = () => reject(request.error);
        });
    }

    async get(storeName, id) {
        return new Promise((resolve, reject) => {
            const transaction = this.db.transaction([storeName], 'readonly');
            const store = transaction.objectStore(storeName);
            const request = store.get(id);
            
            request.onsuccess = () => resolve(request.result);
            request.onerror = () => reject(request.error);
        });
    }

    async delete(storeName, id) {
        return new Promise((resolve, reject) => {
            const transaction = this.db.transaction([storeName], 'readwrite');
            const store = transaction.objectStore(storeName);
            const request = store.delete(id);
            
            request.onsuccess = () => resolve(true);
            request.onerror = () => reject(request.error);
        });
    }

    async query(storeName, indexName, value) {
        return new Promise((resolve, reject) => {
            const transaction = this.db.transaction([storeName], 'readonly');
            const store = transaction.objectStore(storeName);
            const index = store.index(indexName);
            const request = index.getAll(value);
            
            request.onsuccess = () => resolve(request.result || []);
            request.onerror = () => reject(request.error);
        });
    }

    async count(storeName) {
        return new Promise((resolve, reject) => {
            const transaction = this.db.transaction([storeName], 'readonly');
            const store = transaction.objectStore(storeName);
            const request = store.count();
            
            request.onsuccess = () => resolve(request.result);
            request.onerror = () => reject(request.error);
        });
    }

    async clear(storeName) {
        return new Promise((resolve, reject) => {
            const transaction = this.db.transaction([storeName], 'readwrite');
            const store = transaction.objectStore(storeName);
            const request = store.clear();
            
            request.onsuccess = () => resolve(true);
            request.onerror = () => reject(request.error);
        });
    }

    async exists(storeName, id) {
        const item = await this.get(storeName, id);
        return !!item;
    }

    async saveSettings(settings) {
        const promises = Object.entries(settings).map(([key, value]) => 
            this.save('settings', { key, value })
        );
        await Promise.all(promises);
    }

    async getSettings() {
        const settingsArray = await this.getAll('settings');
        const settings = {};
        settingsArray.forEach(item => {
            settings[item.key] = item.value;
        });
        return settings;
    }

    async getSetting(key) {
        const item = await this.get('settings', key);
        return item ? item.value : null;
    }

    async deleteSetting(key) {
        return this.delete('settings', key);
    }

    async backup() {
        const backup = {
            version: this.version,
            timestamp: new Date().toISOString(),
            data: {}
        };

        for (const storeName of Object.keys(this.stores)) {
            backup.data[storeName] = await this.getAll(storeName);
        }

        return backup;
    }

    async restore(backup) {
        if (!backup.data) {
            throw new Error('Invalid backup format');
        }

        for (const [storeName, items] of Object.entries(backup.data)) {
            if (this.stores[storeName]) {
                await this.clear(storeName);
                for (const item of items) {
                    await this.save(storeName, item);
                }
            }
        }
    }

    async getStorageInfo() {
        const info = {};
        for (const storeName of Object.keys(this.stores)) {
            info[storeName] = await this.count(storeName);
        }
        return info;
    }

    close() {
        if (this.db) {
            this.db.close();
            this.db = null;
        }
    }

    async deleteDatabase() {
        this.close();
        return new Promise((resolve, reject) => {
            const deleteRequest = indexedDB.deleteDatabase(this.dbName);
            deleteRequest.onsuccess = () => resolve(true);
            deleteRequest.onerror = () => reject(deleteRequest.error);
        });
    }
}