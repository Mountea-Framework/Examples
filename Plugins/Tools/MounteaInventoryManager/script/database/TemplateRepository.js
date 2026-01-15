import { Helpers } from '../utils/Helpers.js';

export class TemplateRepository {
    constructor(dbManager) {
        this.dbManager = dbManager;
    }

    createEmptyTemplate() {
        return {
            id: Helpers.generateGUID(),
            itemName: '',
            itemID: Helpers.generateGUID(),
            displayName: '',
            thumbnailDescription: '',
            description: '',
            itemType: 'Misc',
            itemSubCategory: '',
            rarity: 'Common',
            maxStackSize: 1,
            maxQuantity: 0,
            weight: 0,
            value: 0,
            durability: 100,
            tags: [],
            bHasWeight: false,
            bHasPrice: false,
            bHasDurability: false,
            basePrice: 0,
            sellPriceCoefficient: 0.5,
            maxDurability: 100,
            baseDurability: 100,
            durabilityPenalization: 1.0,
            durabilityToPriceCoefficient: 1.0,
            isStackable: false,
            isDroppable: true,
            isUsable: false,
            isEquippable: false,
            isTradeable: true,
            isQuestItem: false,
            iconFileId: null,
            meshFileId: null,
            meshPath: '',
            materials: [],
            equipSlot: 'none',
            customProperties: [],
            createdAt: new Date().toISOString(),
            updatedAt: new Date().toISOString()
        };
    }

    async getAll() {
        try {
            const templates = await this.dbManager.getAll('templates');
            return templates.map(template => this.migrateTemplate(template));
        } catch (error) {
            console.error('Error loading templates:', error);
            return [];
        }
    }

    async get(id) {
        try {
            const template = await this.dbManager.get('templates', id);
            return template ? this.migrateTemplate(template) : null;
        } catch (error) {
            console.error('Error loading template:', error);
            return null;
        }
    }

    async save(template) {
        try {
            template.updatedAt = new Date().toISOString();
            if (!template.createdAt) {
                template.createdAt = template.updatedAt;
            }

            template = this.validateTemplate(template);
            await this.dbManager.save('templates', template);
            return template;
        } catch (error) {
            console.error('Error saving template:', error);
            throw error;
        }
    }

    async delete(id) {
        try {
            await this.dbManager.delete('templates', id);
            return true;
        } catch (error) {
            console.error('Error deleting template:', error);
            return false;
        }
    }

    async deleteMultiple(ids) {
        try {
            const promises = ids.map(id => this.dbManager.deleteTemplate(id));
            await Promise.all(promises);
            return true;
        } catch (error) {
            console.error('Error deleting multiple templates:', error);
            return false;
        }
    }

    async duplicate(id) {
        try {
            const original = await this.get(id);
            if (!original) {
                throw new Error('Template not found');
            }

            const duplicate = { ...original };
            duplicate.id = Helpers.generateGUID();
            duplicate.itemID = Helpers.generateGUID();
            duplicate.itemName = `${original.itemName} (Copy)`;
            duplicate.displayName = `${original.displayName} (Copy)`;
            duplicate.createdAt = new Date().toISOString();
            duplicate.updatedAt = duplicate.createdAt;

            if (duplicate.materials) {
                duplicate.materials = duplicate.materials.map(material => ({ ...material }));
            }

            if (duplicate.customProperties) {
                duplicate.customProperties = duplicate.customProperties.map(prop => ({ ...prop }));
            }

            if (duplicate.tags) {
                duplicate.tags = [...duplicate.tags];
            }

            await this.save(duplicate);
            return duplicate;
        } catch (error) {
            console.error('Error duplicating template:', error);
            throw error;
        }
    }

    validateTemplate(template) {
        const validated = { ...template };

        if (!validated.itemName || validated.itemName.trim() === '') {
            validated.itemName = 'Unnamed Template';
        }

        if (!validated.itemID || validated.itemID.trim() === '') {
            validated.itemID = Helpers.generateGUID();
        }

        if (!validated.displayName || validated.displayName.trim() === '') {
            validated.displayName = validated.itemName;
        }

        if (!validated.itemType) {
            validated.itemType = 'Misc';
        }

        if (!validated.rarity) {
            validated.rarity = 'Common';
        }

        if (!validated.equipSlot) {
            validated.equipSlot = 'none';
        }

        if (!Array.isArray(validated.tags)) {
            validated.tags = [];
        }

        if (!Array.isArray(validated.materials)) {
            validated.materials = [];
        }

        if (!Array.isArray(validated.customProperties)) {
            validated.customProperties = [];
        }

        validated.materials = validated.materials.filter(material => 
            material && material.name && material.path
        );

        validated.customProperties = validated.customProperties.filter(prop => 
            prop && prop.name && prop.value
        );

        validated.maxStackSize = Math.max(1, parseInt(validated.maxStackSize) || 1);
        validated.maxQuantity = Math.max(0, parseInt(validated.maxQuantity) || 0);
        validated.weight = Math.max(0, parseFloat(validated.weight) || 0);
        validated.value = Math.max(0, parseInt(validated.value) || 0);
        validated.durability = Math.max(0, parseInt(validated.durability) || 100);
        validated.basePrice = Math.max(0, parseFloat(validated.basePrice) || 0);
        validated.sellPriceCoefficient = Math.max(0, parseFloat(validated.sellPriceCoefficient) || 0.5);
        validated.maxDurability = Math.max(1, parseInt(validated.maxDurability) || 100);
        validated.baseDurability = Math.max(0, parseInt(validated.baseDurability) || 100);
        validated.durabilityPenalization = Math.max(0, parseFloat(validated.durabilityPenalization) || 1.0);
        validated.durabilityToPriceCoefficient = Math.max(0, parseFloat(validated.durabilityToPriceCoefficient) || 1.0);

        validated.bHasWeight = Boolean(validated.bHasWeight);
        validated.bHasPrice = Boolean(validated.bHasPrice);
        validated.bHasDurability = Boolean(validated.bHasDurability);
        validated.isStackable = Boolean(validated.isStackable);
        validated.isDroppable = Boolean(validated.isDroppable);
        validated.isUsable = Boolean(validated.isUsable);
        validated.isEquippable = Boolean(validated.isEquippable);
        validated.isTradeable = Boolean(validated.isTradeable);
        validated.isQuestItem = Boolean(validated.isQuestItem);

        return validated;
    }

    migrateTemplate(template) {
        const migrated = { ...template };

        if (migrated.materialPath && !migrated.materials) {
            const materialName = this.extractMaterialNameFromPath(migrated.materialPath);
            migrated.materials = [{
                name: materialName,
                path: migrated.materialPath
            }];
            delete migrated.materialPath;
        }

        if (!Array.isArray(migrated.materials)) {
            migrated.materials = [];
        }

        if (!Array.isArray(migrated.tags)) {
            migrated.tags = [];
        }

        if (!Array.isArray(migrated.customProperties)) {
            migrated.customProperties = [];
        }

        if (migrated.thumbnailDescription === undefined && migrated['short-description']) {
            migrated.thumbnailDescription = migrated['short-description'];
            delete migrated['short-description'];
        }

        if (!migrated.equipSlot) {
            migrated.equipSlot = 'none';
        }

        if (!migrated.itemSubCategory) {
            migrated.itemSubCategory = '';
        }

        if (migrated.bHasWeight === undefined) {
            migrated.bHasWeight = false;
        }

        if (migrated.bHasPrice === undefined) {
            migrated.bHasPrice = false;
        }

        if (migrated.bHasDurability === undefined) {
            migrated.bHasDurability = false;
        }

        if (!migrated.createdAt) {
            migrated.createdAt = new Date().toISOString();
        }

        if (!migrated.updatedAt) {
            migrated.updatedAt = migrated.createdAt;
        }

        return migrated;
    }

    extractMaterialNameFromPath(path) {
        if (!path) return 'Material';
        
        const parts = path.split('/');
        const fileName = parts[parts.length - 1];
        
        return fileName || 'Material';
    }

    async exportTemplates(templateIds = null) {
        try {
            let templates;
            if (templateIds && templateIds.length > 0) {
                templates = await Promise.all(
                    templateIds.map(id => this.get(id))
                );
                templates = templates.filter(t => t !== null);
            } else {
                templates = await this.getAll();
            }

            return {
                version: '1.1.0',
                exportDate: new Date().toISOString(),
                templates: templates.map(template => this.prepareForExport(template))
            };
        } catch (error) {
            console.error('Error exporting templates:', error);
            throw error;
        }
    }

    prepareForExport(template) {
        const exported = { ...template };
        
        delete exported.createdAt;
        delete exported.updatedAt;
        
        if (exported.iconFileId) {
            exported.hasIcon = true;
            delete exported.iconFileId;
        }
        
        if (exported.meshFileId) {
            exported.hasMesh = true;
            delete exported.meshFileId;
        }
        
        return exported;
    }

    async importTemplates(data) {
        try {
            if (!data.templates || !Array.isArray(data.templates)) {
                throw new Error('Invalid import data format');
            }

            const importedTemplates = [];
            const errors = [];

            for (const templateData of data.templates) {
                try {
                    const template = this.prepareForImport(templateData);
                    const savedTemplate = await this.save(template);
                    importedTemplates.push(savedTemplate);
                } catch (error) {
                    errors.push({
                        template: templateData.itemName || 'Unknown',
                        error: error.message
                    });
                }
            }

            return {
                imported: importedTemplates,
                errors: errors,
                total: data.templates.length
            };
        } catch (error) {
            console.error('Error importing templates:', error);
            throw error;
        }
    }

    prepareForImport(templateData) {
        const template = this.migrateTemplate(templateData);
        
        template.id = Helpers.generateGUID();
        template.itemID = Helpers.generateGUID();
        template.createdAt = new Date().toISOString();
        template.updatedAt = template.createdAt;
        
        delete template.iconFileId;
        delete template.meshFileId;
        delete template.hasIcon;
        delete template.hasMesh;
        
        return this.validateTemplate(template);
    }

    async search(query) {
        try {
            const templates = await this.getAll();
            const searchTerm = query.toLowerCase();
            
            return templates.filter(template => 
                template.itemName.toLowerCase().includes(searchTerm) ||
                template.displayName.toLowerCase().includes(searchTerm) ||
                template.description.toLowerCase().includes(searchTerm) ||
                template.itemType.toLowerCase().includes(searchTerm) ||
                template.tags.some(tag => tag.toLowerCase().includes(searchTerm))
            );
        } catch (error) {
            console.error('Error searching templates:', error);
            return [];
        }
    }

    async getByType(itemType) {
        try {
            const templates = await this.getAll();
            return templates.filter(template => template.itemType === itemType);
        } catch (error) {
            console.error('Error filtering templates by type:', error);
            return [];
        }
    }

    async getByRarity(rarity) {
        try {
            const templates = await this.getAll();
            return templates.filter(template => template.rarity === rarity);
        } catch (error) {
            console.error('Error filtering templates by rarity:', error);
            return [];
        }
    }
}