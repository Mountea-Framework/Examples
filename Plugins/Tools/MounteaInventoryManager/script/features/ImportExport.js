import { Helpers } from "../utils/Helpers.js";

export class ImportExport {
    constructor(editor) {
        this.editor = editor;
    }

    async importTemplate() {
        document.getElementById('fileInput').click();
    }

    async handleFileImport(event) {
        const file = event.target.files[0];
        if (!file) return;

        const reader = new FileReader();
        reader.onload = async (e) => {
            try {
                const imported = JSON.parse(e.target.result);

                if (Array.isArray(imported)) {
                    await this.importMultipleTemplates(imported);
                } else {
                    await this.importSingleTemplate(imported);
                }
            } catch (error) {
                this.editor.notifications.show('Invalid JSON file', 'error');
            }
        };

        reader.readAsText(file);
        event.target.value = '';
    }

    async importSingleTemplate(templateData) {
        templateData.id = templateData.itemID;
        templateData.iconFileId = null;
        templateData.meshFileId = null;

        if (templateData.materialPath && !templateData.materials) {
            const materialName = this.extractMaterialNameFromPath(templateData.materialPath);
            templateData.materials = [{
                name: materialName,
                path: templateData.materialPath
            }];
            delete templateData.materialPath;
        }

        if (!Array.isArray(templateData.materials)) {
            templateData.materials = [];
        }

        this.editor.templates.unshift(templateData);
        this.editor.currentTemplate = templateData;
        
        await this.editor.templateRepo.save(templateData);
        this.editor.ui.loadTemplateToForm(templateData);
        this.editor.ui.selectTemplateInList(templateData.id);
        this.editor.ui.renderTemplatesList();
        this.editor.notifications.show('Template imported successfully', 'success');
    }

    async importMultipleTemplates(templatesData) {
        const imported = await this.editor.templateRepo.import(templatesData);
        this.editor.templates.unshift(...imported);
        this.editor.ui.renderTemplatesList();
        this.editor.ui.updateSelectionUI();
        this.editor.notifications.show(`Imported ${imported.length} templates`, 'success');
    }

    async exportTemplates() {
        if (this.editor.templates.length === 0) {
            this.editor.notifications.show('No templates to export', 'error');
            return;
        }

        const exportData = await this.editor.templateRepo.export();
        this.downloadJSON(exportData, 'inventory_templates.json');
        this.editor.notifications.show('Templates exported successfully', 'success');
    }

    async exportSingleTemplate() {
        if (!this.editor.currentTemplate) {
            this.editor.notifications.show('No template selected', 'error');
            return;
        }

        try {
            const JSZip = await this.loadJSZip();
            const zip = new JSZip();

            const { iconFileId, meshFileId, ...exportTemplate } = this.editor.currentTemplate;
            zip.file('template.json', JSON.stringify(exportTemplate, null, 2));

            const iconFolder = zip.folder('Icon');
            const meshFolder = zip.folder('Mesh');

            await this.addFileToZip(iconFolder, this.editor.currentTemplate.iconFileId, 'icon');
            await this.addFileToZip(meshFolder, this.editor.currentTemplate.meshFileId, 'mesh');

            const content = await zip.generateAsync({ type: 'blob' });
            this.downloadBlob(content, `${this.editor.currentTemplate.itemID || 'template'}.mnteaitem`);
            this.editor.notifications.show('Single template exported successfully', 'success');
        } catch (error) {
            console.error('Export error:', error);
            this.editor.notifications.show('Export failed. JSZip library required.', 'error');
        }
    }

    async exportMultipleTemplates() {
        if (this.editor.templates.length === 0) {
            this.editor.notifications.show('No templates to export', 'error');
            return;
        }

        try {
            const JSZip = await this.loadJSZip();
            const zip = new JSZip();

            const templatesToExport = this.editor.selectedTemplates.size > 0
                ? this.editor.templates.filter(t => this.editor.selectedTemplates.has(t.id))
                : this.editor.templates;

            if (templatesToExport.length === 0) {
                this.editor.notifications.show('No templates to export', 'error');
                return;
            }

            for (const template of templatesToExport) {
                const itemId = template.itemID || template.id;
                const templateFolder = zip.folder(itemId);

                const { iconFileId, meshFileId, ...exportTemplate } = template;
                templateFolder.file('template.json', JSON.stringify(exportTemplate, null, 2));

                const iconFolder = templateFolder.folder('Icon');
                const meshFolder = templateFolder.folder('Mesh');

                await this.addFileToZip(iconFolder, template.iconFileId, 'icon');
                await this.addFileToZip(meshFolder, template.meshFileId, 'mesh');
            }

            const content = await zip.generateAsync({ type: 'blob' });
            this.downloadBlob(content, 'inventory_templates.mnteaitems');

            const exportCount = templatesToExport.length;
            const message = this.editor.selectedTemplates.size > 0
                ? `${exportCount} selected templates exported successfully`
                : `${exportCount} templates exported successfully`;
            this.editor.notifications.show(message, 'success');
        } catch (error) {
            console.error('Export error:', error);
            this.editor.notifications.show('Export failed. JSZip library required.', 'error');
        }
    }

    async addFileToZip(folder, fileId, fileType) {
        if (!fileId) {
            const readmeText = fileType === 'icon'
                ? 'Place your icon image file here (PNG, JPG, BMP)'
                : 'Place your mesh file here (FBX, OBJ, etc.)';
            folder.file('README.txt', readmeText);
            return;
        }

        const fileData = await this.editor.fileRepo.get(fileId);
        if (!fileData) {
            const readmeText = fileType === 'icon'
                ? 'Place your icon image file here (PNG, JPG, BMP)'
                : 'Place your mesh file here (FBX, OBJ, etc.)';
            folder.file('README.txt', readmeText);
            return;
        }

        folder.file(fileData.fileName, fileData.data);
    }

    async loadJSZip() {
        if (window.JSZip) {
            return window.JSZip;
        }

        return new Promise((resolve, reject) => {
            const script = document.createElement('script');
            script.src = 'https://cdnjs.cloudflare.com/ajax/libs/jszip/3.10.1/jszip.min.js';
            script.onload = () => resolve(window.JSZip);
            script.onerror = () => reject(new Error('Failed to load JSZip'));
            document.head.appendChild(script);
        });
    }

    downloadJSON(data, filename) {
        const dataStr = JSON.stringify(data, null, 2);
        const dataBlob = new Blob([dataStr], { type: 'application/json' });
        this.downloadBlob(dataBlob, filename);
    }

    downloadBlob(blob, filename) {
        const link = document.createElement('a');
        link.href = URL.createObjectURL(blob);
        link.download = filename;
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
        URL.revokeObjectURL(link.href);
    }

    async exportAsDataTable() {
        if (this.editor.templates.length === 0) {
            this.editor.notifications.show('No templates to export', 'error');
            return;
        }

        const headers = [
            'Name', 'ItemID', 'DisplayName', 'ThumbnailDescription', 'Description',
            'ItemType', 'Rarity', 'MaxStackSize', 'Weight', 'Value', 'Durability',
            'bIsStackable', 'bIsDroppable', 'bIsUsable', 'bIsEquippable', 
            'bIsTradeable', 'bIsQuestItem', 'MeshPath', 'Materials', 'EquipSlot'
        ];

        let csvContent = headers.join(',') + '\n';

        this.editor.templates.forEach(template => {
            const materialsStr = template.materials && template.materials.length > 0 
                ? template.materials.map(m => `${m.name}:${m.path}`).join(';')
                : '';

            const row = [
                `"${template.itemName || ''}"`,
                `"${template.itemID || ''}"`,
                `"${template.displayName || ''}"`,
                `"${(template.thumbnailDescription || '').replace(/"/g, '""')}"`,
                `"${(template.description || '').replace(/"/g, '""')}"`,
                `"${template.itemType || 'Misc'}"`,
                `"${template.rarity || 'Common'}"`,
                template.maxStackSize || 1,
                template.weight || 0,
                template.value || 0,
                template.durability || 100,
                template.isStackable ? 'True' : 'False',
                template.isDroppable ? 'True' : 'False',
                template.isUsable ? 'True' : 'False',
                template.isEquippable ? 'True' : 'False',
                template.isTradeable ? 'True' : 'False',
                template.isQuestItem ? 'True' : 'False',
                `"${template.meshPath || ''}"`,
                `"${materialsStr}"`,
                `"${template.equipSlot || 'None'}"`
            ];

            csvContent += row.join(',') + '\n';
        });

        const dataBlob = new Blob([csvContent], { type: 'text/csv' });
        this.downloadBlob(dataBlob, 'InventoryItemTemplates.csv');
        this.editor.notifications.show('Data table exported successfully', 'success');
    }

    async importFromCSV(file) {
        return new Promise((resolve, reject) => {
            const reader = new FileReader();
            reader.onload = (e) => {
                try {
                    const csv = e.target.result;
                    const lines = csv.split('\n');
                    const headers = lines[0].split(',').map(h => h.replace(/"/g, '').trim());
                    const templates = [];

                    for (let i = 1; i < lines.length; i++) {
                        if (!lines[i].trim()) continue;
                        
                        const values = this.parseCSVLine(lines[i]);
                        const template = this.csvRowToTemplate(headers, values);
                        if (template) templates.push(template);
                    }

                    resolve(templates);
                } catch (error) {
                    reject(error);
                }
            };
            reader.onerror = reject;
            reader.readAsText(file);
        });
    }

    parseCSVLine(line) {
        const result = [];
        let current = '';
        let inQuotes = false;

        for (let i = 0; i < line.length; i++) {
            const char = line[i];
            
            if (char === '"') {
                if (inQuotes && line[i + 1] === '"') {
                    current += '"';
                    i++;
                } else {
                    inQuotes = !inQuotes;
                }
            } else if (char === ',' && !inQuotes) {
                result.push(current);
                current = '';
            } else {
                current += char;
            }
        }
        
        result.push(current);
        return result;
    }

    csvRowToTemplate(headers, values) {
        const template = {};
        
        for (let i = 0; i < headers.length && i < values.length; i++) {
            const header = headers[i].toLowerCase();
            let value = values[i].replace(/"/g, '').trim();
            
            switch (header) {
                case 'name':
                    template.itemName = value;
                    break;
                case 'itemid':
                    template.itemID = value;
                    template.id = value;
                    break;
                case 'displayname':
                    template.displayName = value;
                    break;
                case 'thumbnaildescription':
                    template.thumbnailDescription = value;
                    break;
                case 'description':
                    template.description = value;
                    break;
                case 'itemtype':
                    template.itemType = value;
                    break;
                case 'rarity':
                    template.rarity = value;
                    break;
                case 'maxstacksize':
                    template.maxStackSize = parseInt(value) || 1;
                    break;
                case 'weight':
                    template.weight = parseFloat(value) || 0;
                    break;
                case 'value':
                    template.value = parseInt(value) || 0;
                    break;
                case 'durability':
                    template.durability = parseInt(value) || 100;
                    break;
                case 'bisstackable':
                    template.isStackable = value.toLowerCase() === 'true';
                    break;
                case 'bisdroppable':
                    template.isDroppable = value.toLowerCase() === 'true';
                    break;
                case 'bisusable':
                    template.isUsable = value.toLowerCase() === 'true';
                    break;
                case 'bisequippable':
                    template.isEquippable = value.toLowerCase() === 'true';
                    break;
                case 'bistradeable':
                    template.isTradeable = value.toLowerCase() === 'true';
                    break;
                case 'bisquestitem':
                    template.isQuestItem = value.toLowerCase() === 'true';
                    break;
                case 'meshpath':
                    template.meshPath = value;
                    break;
                case 'materialpath':
                    if (value) {
                        const materialName = this.extractMaterialNameFromPath(value);
                        template.materials = [{
                            name: materialName,
                            path: value
                        }];
                    }
                    break;
                case 'materials':
                    if (value) {
                        template.materials = value.split(';').map(material => {
                            const [name, path] = material.split(':');
                            return { name: name || 'Material', path: path || `/Game/Materials/${name || 'Material'}` };
                        });
                    }
                    break;
                case 'equipslot':
                    template.equipSlot = value;
                    break;
            }
        }

        if (!Array.isArray(template.materials)) {
            template.materials = [];
        }
        
        if (!template.itemID) {
            template.itemID = Helpers.generateGUID();
            template.id = template.itemID;
        }
        
        return template;
    }

    extractMaterialNameFromPath(path) {
        if (!path) return 'Material';
        
        const parts = path.split('/');
        const fileName = parts[parts.length - 1];
        
        return fileName || 'Material';
    }

    generateGUID() {
        return 'xxxxxxxx-xxxx-xxxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
            const r = Math.random() * 16 | 0;
            const v = c == 'x' ? r : (r & 0x3) | 0x8;
            return v.toString(16);
        });
    }

    async createBackup() {
        try {
            const backup = await this.editor.dbManager.backup();
            this.downloadJSON(backup, `inventory_backup_${new Date().toISOString().split('T')[0]}.json`);
            this.editor.notifications.show('Backup created successfully', 'success');
        } catch (error) {
            console.error('Backup failed:', error);
            this.editor.notifications.show('Failed to create backup', 'error');
        }
    }

    async restoreBackup(file) {
        try {
            const reader = new FileReader();
            reader.onload = async (e) => {
                try {
                    const backupData = JSON.parse(e.target.result);
                    await this.editor.dbManager.restore(backupData);
                    await this.editor.loadTemplates();
                    this.editor.ui.renderTemplatesList();
                    this.editor.notifications.show('Backup restored successfully', 'success');
                } catch (error) {
                    console.error('Restore failed:', error);
                    this.editor.notifications.show('Failed to restore backup', 'error');
                }
            };
            reader.readAsText(file);
        } catch (error) {
            console.error('Restore failed:', error);
            this.editor.notifications.show('Failed to restore backup', 'error');
        }
    }
}