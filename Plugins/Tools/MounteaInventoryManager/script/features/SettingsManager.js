export class SettingsManager {
    constructor(editor) {
        this.editor = editor;
    }

    populateDropdowns() {
        this.populateItemTypes();
        this.populateSubcategories();
        this.populateRarities();
        this.populateEquipmentSlots();
    }

    populateItemTypes() {
        const select = document.getElementById('itemType');
        select.innerHTML = '';
        
        this.editor.settings.categories.forEach(category => {
            const option = document.createElement('option');
            option.value = category;
            option.textContent = category;
            select.appendChild(option);
        });
    }

    populateSubcategories() {
        const select = document.getElementById('itemSubCategory');
        const category = document.getElementById('itemType').value;
        
        select.innerHTML = '<option value="">None</option>';
        
        const subcategories = this.editor.settings.subcategories[category];
        if (subcategories) {
            subcategories.forEach(subcategory => {
                const option = document.createElement('option');
                option.value = subcategory;
                option.textContent = subcategory;
                select.appendChild(option);
            });
        }
    }

    populateRarities() {
        const select = document.getElementById('rarity');
        select.innerHTML = '';
        
        this.editor.settings.rarities.forEach(rarity => {
            const option = document.createElement('option');
            option.value = rarity.name;
            option.textContent = rarity.name;
            option.style.color = rarity.color;
            select.appendChild(option);
        });
    }

    populateEquipmentSlots() {
        const select = document.getElementById('equipSlot');
        select.innerHTML = '';
        
        this.editor.settings.equipmentSlots.forEach(slot => {
            const option = document.createElement('option');
            option.value = slot;
            option.textContent = slot;
            select.appendChild(option);
        });
    }

    showSettings() {
        this.populateSettingsModal();
        document.getElementById('settingsModal').classList.add('show');
    }

    closeSettings() {
        document.getElementById('settingsModal').classList.remove('show');
    }

    populateSettingsModal() {
        this.populateCategoriesContainer();
        this.populateSubcategoriesContainer();
        this.populateRaritiesContainer();
        this.populateEquipmentSlotsContainer();
    }

    populateCategoriesContainer() {
        const container = document.getElementById('categoriesContainer');
        container.innerHTML = '';
        
        this.editor.settings.categories.forEach((category, index) => {
            const row = this.createCategoryRow(category);
            container.appendChild(row);
        });
    }

    populateRaritiesContainer() {
        const container = document.getElementById('raritiesContainer');
        container.innerHTML = '';
        
        this.editor.settings.rarities.forEach((rarity, index) => {
            const row = this.createRarityRow(rarity.name, rarity.color);
            container.appendChild(row);
        });
    }

    populateEquipmentSlotsContainer() {
        const container = document.getElementById('equipmentSlotsContainer');
        container.innerHTML = '';
        
        this.editor.settings.equipmentSlots.forEach((slot, index) => {
            const row = this.createEquipmentSlotRow(slot);
            container.appendChild(row);
        });
    }

    createCategoryRow(category) {
        const row = document.createElement('div');
        row.className = 'settings-row';
        row.innerHTML = `
            <input type="text" value="${category}" class="category-input">
            <button type="button" class="btn btn-danger btn-small close-small">✖</button>
        `;
        
        row.querySelector('button').addEventListener('click', () => row.remove());
        return row;
    }

    createRarityRow(name, color) {
        const row = document.createElement('div');
        row.className = 'settings-row';
        row.innerHTML = `
            <input type="text" value="${name}" class="rarity-name-input">
            <input type="color" value="${color}" class="rarity-color-input">
            <button type="button" class="btn btn-danger btn-small close-small">✖</button>
        `;
        
        row.querySelector('button').addEventListener('click', () => row.remove());
        return row;
    }

    createEquipmentSlotRow(slot) {
        const row = document.createElement('div');
        row.className = 'settings-row';
        row.innerHTML = `
            <input type="text" value="${slot}" class="equipment-slot-input">
            <button type="button" class="btn btn-danger btn-small close-small">✖</button>
        `;
        
        row.querySelector('button').addEventListener('click', () => row.remove());
        return row;
    }

    addCategory() {
        const container = document.getElementById('categoriesContainer');
        const row = this.createCategoryRow('');
        row.querySelector('input').placeholder = 'New Category';
        container.appendChild(row);
    }

    populateSubcategoriesContainer() {
        const container = document.getElementById('subcategoriesContainer');
        container.innerHTML = '';
        
        Object.entries(this.editor.settings.subcategories).forEach(([category, subs]) => {
            const categorySection = document.createElement('div');
            categorySection.className = 'settings-container';
            categorySection.innerHTML = `<h4>${category}</h4>`;

            // TODO: Wrap in `settings-controls` div
            const settingsControl = document.createElement('div');
            settingsControl.className = 'settings-controls';

            const addBtn = document.createElement('button');
            addBtn.type = 'button';
            addBtn.className = 'btn btn-secondary';
            addBtn.textContent = `Add ${category} Subcategory`;
            addBtn.onclick = () => this.addSubcategory(category);
            settingsControl.appendChild(addBtn);
            categorySection.appendChild(settingsControl);
            
            container.appendChild(categorySection);
            
            subs.forEach(sub => {
                const row = this.createSubcategoryRow(category, sub);
                categorySection.appendChild(row);
            });
        });
    }

    createSubcategoryRow(category, subcategory) {
        const row = document.createElement('div');
        row.className = 'settings-row';
        row.innerHTML = `
            <input type="text" value="${subcategory}" class="subcategory-input" data-category="${category}">
            <button type="button" class="btn btn-danger btn-small close-small">✖</button>
        `;
        
        row.querySelector('button').addEventListener('click', () => row.remove());
        return row;
    }

    addSubcategory(category) {
        const container = document.getElementById('subcategoriesContainer');
        const categorySection = [...container.children].find(section => 
            section.querySelector('h5')?.textContent === category
        );
        
        if (categorySection) {
            const row = this.createSubcategoryRow(category, '');
            row.querySelector('input').placeholder = `New ${category} Subcategory`;
            categorySection.insertBefore(row, categorySection.lastElementChild);
        }
    }

    collectSubcategories() {
        const container = document.getElementById('subcategoriesContainer');
        const subcategories = {};
        
        this.editor.settings.categories.forEach(category => {
            const inputs = container.querySelectorAll(`input[data-category="${category}"]`);
            subcategories[category] = Array.from(inputs)
                .map(input => input.value.trim())
                .filter(value => value.length > 0);
        });
        
        return subcategories;
    }

    addRarity() {
        const container = document.getElementById('raritiesContainer');
        const row = this.createRarityRow('', '#000000');
        row.querySelector('.rarity-name-input').placeholder = 'New Rarity';
        container.appendChild(row);
    }

    addEquipmentSlot() {
        const container = document.getElementById('equipmentSlotsContainer');
        const row = this.createEquipmentSlotRow('');
        row.querySelector('input').placeholder = 'New Equipment Slot';
        container.appendChild(row);
    }

    async saveSettingsData() {
        try {
            const newSettings = this.collectSettingsFromModal();
            
            const validation = this.validateSettings(newSettings);
            if (!validation.isValid) {
                this.editor.notifications.show(validation.error, 'error');
                return;
            }

            this.editor.settings = newSettings;
            await this.editor.dbManager.saveSettings(newSettings);
            this.populateDropdowns();
            this.closeSettings();
            this.editor.notifications.show('Settings saved successfully', 'success');
        } catch (error) {
            console.error('Settings save error:', error);
            this.editor.notifications.show('Failed to save settings', 'error');
        }
    }

    collectSettingsFromModal() {
        return {
            categories: this.collectCategories(),
            subcategories: this.collectSubcategories(),
            rarities: this.collectRarities(),
            equipmentSlots: this.collectEquipmentSlots()
        };
    }

    collectCategories() {
        const container = document.getElementById('categoriesContainer');
        const inputs = container.querySelectorAll('.category-input');
        
        return Array.from(inputs)
            .map(input => input.value.trim())
            .filter(value => value.length > 0);
    }

    collectRarities() {
        const container = document.getElementById('raritiesContainer');
        const rows = container.querySelectorAll('.settings-row');
        
        return Array.from(rows)
            .map(row => {
                const name = row.querySelector('.rarity-name-input').value.trim();
                const color = row.querySelector('.rarity-color-input').value;
                return { name, color };
            })
            .filter(rarity => rarity.name.length > 0);
    }

    collectEquipmentSlots() {
        const container = document.getElementById('equipmentSlotsContainer');
        const inputs = container.querySelectorAll('.equipment-slot-input');
        
        return Array.from(inputs)
            .map(input => input.value.trim())
            .filter(value => value.length > 0);
    }

    validateSettings(settings) {
        if (settings.categories.length === 0) {
            return { isValid: false, error: 'At least one category is required' };
        }

        if (settings.rarities.length === 0) {
            return { isValid: false, error: 'At least one rarity is required' };
        }

        if (settings.equipmentSlots.length === 0) {
            return { isValid: false, error: 'At least one equipment slot is required' };
        }

        // Check for duplicates
        const uniqueCategories = new Set(settings.categories);
        if (uniqueCategories.size !== settings.categories.length) {
            return { isValid: false, error: 'Duplicate categories are not allowed' };
        }

        const uniqueRarityNames = new Set(settings.rarities.map(r => r.name));
        if (uniqueRarityNames.size !== settings.rarities.length) {
            return { isValid: false, error: 'Duplicate rarity names are not allowed' };
        }

        const uniqueSlots = new Set(settings.equipmentSlots);
        if (uniqueSlots.size !== settings.equipmentSlots.length) {
            return { isValid: false, error: 'Duplicate equipment slots are not allowed' };
        }

        return { isValid: true };
    }

    resetToDefaults() {
        if (confirm('Are you sure you want to reset all settings to defaults? This action cannot be undone.')) {
            this.editor.settings = {
                categories: ["Weapon", "Armor", "Consumable", "Material", "Quest", "Misc"],
                rarities: [
                    { name: "Common", color: "#9CA3AF" },
                    { name: "Uncommon", color: "#10B981" },
                    { name: "Rare", color: "#3B82F6" },
                    { name: "Epic", color: "#8B5CF6" },
                    { name: "Legendary", color: "#F59E0B" }
                ],
                equipmentSlots: [
                    "none", "Hand.Left", "Hand.Right", "Back", "Belt", "Helmet",
                    "Chest", "Shoulder.Left", "Shoulder.Right", "Leg.Left", 
                    "Leg.Right", "Boots", "Gloves", "Necklace"
                ]
            };
            
            this.populateSettingsModal();
            this.editor.notifications.show('Settings reset to defaults', 'success');
        }
    }

    exportSettings() {
        const dataStr = JSON.stringify(this.editor.settings, null, 2);
        const dataBlob = new Blob([dataStr], { type: 'application/json' });
        
        const link = document.createElement('a');
        link.href = URL.createObjectURL(dataBlob);
        link.download = 'inventory_settings.json';
        document.body.appendChild(link);
        link.click();
        document.body.removeChild(link);
        URL.revokeObjectURL(link.href);
        
        this.editor.notifications.show('Settings exported successfully', 'success');
    }

    async importSettings(file) {
        try {
            const reader = new FileReader();
            reader.onload = async (e) => {
                try {
                    const importedSettings = JSON.parse(e.target.result);
                    
                    const validation = this.validateSettings(importedSettings);
                    if (!validation.isValid) {
                        this.editor.notifications.show(validation.error, 'error');
                        return;
                    }
                    
                    this.editor.settings = importedSettings;
                    await this.editor.dbManager.saveSettings(importedSettings);
                    this.populateDropdowns();
                    this.populateSettingsModal();
                    this.editor.notifications.show('Settings imported successfully', 'success');
                } catch (error) {
                    this.editor.notifications.show('Invalid settings file', 'error');
                }
            };
            reader.readAsText(file);
        } catch (error) {
            console.error('Settings import error:', error);
            this.editor.notifications.show('Failed to import settings', 'error');
        }
    }

    createSettingsExportDialog() {
        const modal = document.createElement('div');
        modal.className = 'settings-export-modal';
        modal.style.cssText = `
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: rgba(0, 0, 0, 0.5);
            z-index: 10001;
            display: flex;
            align-items: center;
            justify-content: center;
        `;

        const content = document.createElement('div');
        content.style.cssText = `
            background: var(--bg-white);
            border-radius: var(--border-radius-md);
            padding: var(--spacing-lg);
            max-width: 400px;
            width: 90%;
        `;

        content.innerHTML = `
            <h3 style="margin: 0 0 var(--spacing-md) 0;">Export/Import Settings</h3>
            <div style="margin-bottom: var(--spacing-md);">
                <button id="exportSettingsBtn" class="btn btn-primary" style="width: 100%; margin-bottom: var(--spacing-sm);">
                    Export Current Settings
                </button>
                <input type="file" id="importSettingsInput" accept=".json" style="display: none;">
                <button id="importSettingsBtn" class="btn btn-secondary" style="width: 100%; margin-bottom: var(--spacing-sm);">
                    Import Settings
                </button>
                <button id="resetSettingsBtn" class="btn btn-danger" style="width: 100%; margin-bottom: var(--spacing-md);">
                    Reset to Defaults
                </button>
            </div>
            <button id="closeExportDialog" class="btn btn-secondary" style="width: 100%;">
                Close
            </button>
        `;

        modal.appendChild(content);

        content.querySelector('#exportSettingsBtn').addEventListener('click', () => {
            this.exportSettings();
            document.body.removeChild(modal);
        });

        content.querySelector('#importSettingsBtn').addEventListener('click', () => {
            content.querySelector('#importSettingsInput').click();
        });

        content.querySelector('#importSettingsInput').addEventListener('change', (e) => {
            if (e.target.files[0]) {
                this.importSettings(e.target.files[0]);
                document.body.removeChild(modal);
            }
        });

        content.querySelector('#resetSettingsBtn').addEventListener('click', () => {
            this.resetToDefaults();
            document.body.removeChild(modal);
        });

        content.querySelector('#closeExportDialog').addEventListener('click', () => {
            document.body.removeChild(modal);
        });

        document.body.appendChild(modal);
    }

    getSettingsSummary() {
        return {
            categoriesCount: this.editor.settings.categories.length,
            raritiesCount: this.editor.settings.rarities.length,
            equipmentSlotsCount: this.editor.settings.equipmentSlots.length,
            categories: this.editor.settings.categories,
            rarities: this.editor.settings.rarities.map(r => r.name),
            equipmentSlots: this.editor.settings.equipmentSlots
        };
    }

    validateTemplateSetting(template, settingType) {
        switch (settingType) {
            case 'itemType':
                return this.editor.settings.categories.includes(template.itemType);
            case 'rarity':
                return this.editor.settings.rarities.some(r => r.name === template.rarity);
            case 'equipSlot':
                return this.editor.settings.equipmentSlots.includes(template.equipSlot);
            default:
                return true;
        }
    }

    updateTemplatesAfterSettingsChange(oldSettings, newSettings) {
        const updates = [];
        
        this.editor.templates.forEach(template => {
            let needsUpdate = false;
            const updatedTemplate = { ...template };
            
            // Check if item type is still valid
            if (!newSettings.categories.includes(template.itemType)) {
                updatedTemplate.itemType = newSettings.categories[0];
                needsUpdate = true;
            }
            
            // Check if rarity is still valid
            if (!newSettings.rarities.some(r => r.name === template.rarity)) {
                updatedTemplate.rarity = newSettings.rarities[0].name;
                needsUpdate = true;
            }
            
            // Check if equipment slot is still valid
            if (!newSettings.equipmentSlots.includes(template.equipSlot)) {
                updatedTemplate.equipSlot = newSettings.equipmentSlots[0];
                needsUpdate = true;
            }
            
            if (needsUpdate) {
                updates.push(updatedTemplate);
            }
        });
        
        return updates;
    }
}