import { Helpers } from '../utils/Helpers.js';

export class FileRepository {
    constructor(dbManager) {
        this.dbManager = dbManager;
    }

    async save(fileData) {
        try {
            const fileId = Helpers.generateGUID();
            const fileRecord = {
                id: fileId,
                fileName: fileData.fileName,
                fileType: fileData.fileType,
                size: fileData.size,
                data: fileData.data,
                createdAt: new Date().toISOString(),
                updatedAt: new Date().toISOString()
            };
            
            await this.dbManager.save('files', fileRecord);
            return fileId;
        } catch (error) {
            console.error('Failed to store file:', error);
            throw error;
        }
    }

    async get(fileId) {
        try {
            return await this.dbManager.get('files', fileId);
        } catch (error) {
            console.error('Failed to get file:', error);
            return null;
        }
    }

    async getAll() {
        try {
            return await this.dbManager.getAll('files');
        } catch (error) {
            console.error('Failed to get all files:', error);
            return [];
        }
    }

    async delete(fileId) {
        try {
            return await this.dbManager.delete('files', fileId);
        } catch (error) {
            console.error('Failed to delete file:', error);
            return false;
        }
    }

    async deleteMultiple(fileIds) {
        try {
            const promises = fileIds.map(id => this.delete(id));
            await Promise.all(promises);
            return true;
        } catch (error) {
            console.error('Failed to delete multiple files:', error);
            return false;
        }
    }

    async exists(fileId) {
        try {
            const file = await this.get(fileId);
            return !!file;
        } catch (error) {
            return false;
        }
    }

    async getByType(fileType) {
        try {
            return await this.dbManager.query('files', 'fileType', fileType);
        } catch (error) {
            console.error('Failed to get files by type:', error);
            return [];
        }
    }

    async getByName(fileName) {
        try {
            return await this.dbManager.query('files', 'fileName', fileName);
        } catch (error) {
            console.error('Failed to get files by name:', error);
            return [];
        }
    }

    async getPreviewUrl(fileId) {
        try {
            const file = await this.get(fileId);
            if (!file || !file.data) return null;

            const blob = new Blob([file.data], { type: file.fileType });
            return URL.createObjectURL(blob);
        } catch (error) {
            console.error('Failed to create preview URL:', error);
            return null;
        }
    }

    async downloadFile(fileId) {
        try {
            const file = await this.get(fileId);
            if (!file) {
                throw new Error('File not found');
            }

            const blob = new Blob([file.data], { type: file.fileType });
            const url = URL.createObjectURL(blob);
            
            const link = document.createElement('a');
            link.href = url;
            link.download = file.fileName;
            link.style.display = 'none';
            
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
            
            URL.revokeObjectURL(url);
            return true;
        } catch (error) {
            console.error('Failed to download file:', error);
            return false;
        }
    }

    async getFileInfo(fileId) {
        try {
            const file = await this.get(fileId);
            if (!file) return null;

            return {
                id: file.id,
                fileName: file.fileName,
                fileType: file.fileType,
                size: file.size,
                createdAt: file.createdAt,
                updatedAt: file.updatedAt
            };
        } catch (error) {
            console.error('Failed to get file info:', error);
            return null;
        }
    }

    async validateFile(file) {
        const maxSize = 50 * 1024 * 1024; // 50MB
        const allowedTypes = {
            'image/jpeg': ['.jpg', '.jpeg'],
            'image/png': ['.png'],
            'image/bmp': ['.bmp'],
            'model/fbx': ['.fbx'],
            'application/octet-stream': ['.fbx', '.obj']
        };

        if (file.size > maxSize) {
            throw new Error('File size exceeds 50MB limit');
        }

        const isValidType = Object.keys(allowedTypes).includes(file.type) ||
            Object.values(allowedTypes).flat().some(ext => 
                file.name.toLowerCase().endsWith(ext)
            );

        if (!isValidType) {
            throw new Error('File type not supported');
        }

        return true;
    }

    async processFile(file) {
        try {
            await this.validateFile(file);

            return new Promise((resolve, reject) => {
                const reader = new FileReader();
                
                reader.onload = (e) => {
                    resolve({
                        fileName: file.name,
                        fileType: file.type || 'application/octet-stream',
                        size: file.size,
                        data: new Uint8Array(e.target.result)
                    });
                };
                
                reader.onerror = () => reject(new Error('Failed to read file'));
                reader.readAsArrayBuffer(file);
            });
        } catch (error) {
            throw error;
        }
    }

    async getTotalSize() {
        try {
            const files = await this.getAll();
            return files.reduce((total, file) => total + (file.size || 0), 0);
        } catch (error) {
            console.error('Failed to get total size:', error);
            return 0;
        }
    }

    async getStats() {
        try {
            const files = await this.getAll();
            const stats = {
                total: files.length,
                totalSize: 0,
                byType: {},
                averageSize: 0
            };

            files.forEach(file => {
                stats.totalSize += file.size || 0;
                const type = file.fileType || 'unknown';
                stats.byType[type] = (stats.byType[type] || 0) + 1;
            });

            stats.averageSize = files.length > 0 ? stats.totalSize / files.length : 0;
            return stats;
        } catch (error) {
            console.error('Failed to get file stats:', error);
            return { total: 0, totalSize: 0, byType: {}, averageSize: 0 };
        }
    }

    async cleanup() {
        try {
            const files = await this.getAll();
            const cutoffDate = new Date();
            cutoffDate.setDate(cutoffDate.getDate() - 30); // 30 days old

            const oldFiles = files.filter(file => 
                new Date(file.createdAt) < cutoffDate
            );

            if (oldFiles.length > 0) {
                await this.deleteMultiple(oldFiles.map(f => f.id));
                return oldFiles.length;
            }

            return 0;
        } catch (error) {
            console.error('Failed to cleanup files:', error);
            return 0;
        }
    }

    async clear() {
        try {
            return await this.dbManager.clear('files');
        } catch (error) {
            console.error('Failed to clear files:', error);
            return false;
        }
    }
}