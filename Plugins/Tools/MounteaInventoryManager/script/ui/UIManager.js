export class UIManager {
    constructor(editor) {
        this.editor = editor;
    }

    renderTemplatesList() {
        const list = document.getElementById('templatesList');
        if (!list) {
            console.warn('templatesList element not found');
            return;
        }
        
        list.innerHTML = '';

        this.editor.templates.forEach(template => {
            const container = document.createElement('div');
            container.className = 'template-item-container';

            const checkbox = document.createElement('input');
            checkbox.type = 'checkbox';
            checkbox.className = 'template-checkbox';
            checkbox.checked = this.editor.selectedTemplates.has(template.id);
            checkbox.addEventListener('click', e => e.stopPropagation());
            checkbox.addEventListener('change', e => {
                e.stopPropagation();
                this.toggleTemplateSelection(template.id, e.target.checked);
                container.classList.toggle('selected', e.target.checked);
            });

            const button = document.createElement('button');
            const name = template.itemName || 'Unnamed Template';
            const abbr = document.createElement('abbr');
            abbr.title = name;
            abbr.textContent = name.length > 18 ? name.slice(0, 15) + '...' : name;

            button.className = 'template-item';
            button.dataset.templateId = template.id;
            button.addEventListener('click', () => this.selectTemplate(template.id));

            if (this.editor.selectedTemplates.has(template.id)) {
                container.classList.add('selected');
            }

            button.appendChild(abbr);
            button.appendChild(checkbox);
            container.appendChild(button);
            list.appendChild(container);
        });

        this.updateSelectionUI();
    }

    selectTemplate(templateId) {
        if (this.editor.dirtyStateManager) {
            this.editor.dirtyStateManager.checkDirtyBeforeAction('select', () => {
                const template = this.editor.templates.find(t => t.id === templateId);
                if (template) {
                    if (this.editor.currentTemplate?.id === templateId) {
                        this.closeCurrentTemplate();
                        return;
                    }
                    this.editor.currentTemplate = template;
                    this.loadTemplateToForm(template);
                    this.selectTemplateInList(templateId);
                    this.toggleSelectionBaseButtons(true);
                } else {
                    this.toggleSelectionBaseButtons(false);
                }
            });
        } else {
            this.toggleSelectionBaseButtons(false);
        }

        if (this.editor.dirtyStateManager) {
            this.editor.dirtyStateManager.isDirty = false;
            this.editor.dirtyStateManager.updateDirtyIndicators();
        }
    }

    selectTemplateInList(templateId) {
        const buttons = document.querySelectorAll('.template-item');
        buttons.forEach(btn => btn.classList.remove('active'));

        const activeButton = document.querySelector(`[data-template-id="${templateId}"]`);
        if (activeButton) {
            activeButton.classList.add('active');
        }
        
        this.toggleSelectionBaseButtons(true);
    }

    closeCurrentTemplate() {
        if (!this.editor.currentTemplate) return;

        this.clearForm();
        this.editor.currentTemplate = null;
        this.clearTemplateSelection();
        this.updatePreview();
    }

    openMounteaFramework() {
        window.open('https://github.com/Mountea-Framework', '_blank');
    }
    
    clearTemplateSelection() {
        const buttons = document.querySelectorAll('.template-item');
        buttons.forEach(btn => btn.classList.remove('active'));
        this.toggleSelectionBaseButtons(false);
    }

    toggleSelectionBaseButtons(isVisible) {
        const buttons = document.querySelectorAll('.selection');
        buttons.forEach(btn => btn.classList.toggle('hidden', !isVisible));
    }

    toggleTemplateSelection(templateId, isSelected) {
        if (isSelected) {
            this.editor.selectedTemplates.add(templateId);
        } else {
            this.editor.selectedTemplates.delete(templateId);
        }
        this.updateSelectionUI();
    }

    selectAllTemplates() {
        this.editor.selectedTemplates.clear();
        this.editor.templates.forEach(template => {
            this.editor.selectedTemplates.add(template.id);
        });
        this.updateSelectionUI();
        this.renderTemplatesList();
    }

    deselectAllTemplates() {
        this.editor.selectedTemplates.clear();
        this.updateSelectionUI();
        this.renderTemplatesList();
    }

    updateSelectionUI() {
        const count = this.editor.selectedTemplates.size;
        const total = this.editor.templates.length;

        const selectionCount = document.getElementById('selectionCount');
        if (selectionCount) {
            selectionCount.textContent = `${count} of ${total} selected`;
        }

        const selectAllCheckbox = document.getElementById('selectAllTemplates');
        if (selectAllCheckbox) {
            if (count === 0) {
                selectAllCheckbox.checked = false;
                selectAllCheckbox.indeterminate = false;
            } else if (count === total) {
                selectAllCheckbox.checked = true;
                selectAllCheckbox.indeterminate = false;
            } else {
                selectAllCheckbox.checked = false;
                selectAllCheckbox.indeterminate = true;
            }
        }

        const exportButton = document.getElementById('exportMultiple');
        if (exportButton) {
            const exportTitle = exportButton.querySelector('abbr');
            if (exportTitle) {
                if (count === 0) {
                    exportTitle.textContent = 'Export All Items';
                    exportButton.title = 'Exports all Templates to Unreal .mnteaitems file';
                } else {
                    exportTitle.textContent = `Export Items (${count})`;
                    exportButton.title = `Exports ${count} selected Templates to Unreal .mnteaitems file`;
                }
            }
        }
    }

    async loadTemplateToForm(template) {
        this.clearAllFormErrors();

        const setValue = (id, value) => {
            const element = document.getElementById(id);
            if (element) element.value = value || '';
        };

        const setChecked = (id, checked) => {
            const element = document.getElementById(id);
            if (element) element.checked = checked || false;
        };

        setValue('itemName', template.itemName);
        setValue('itemID', template.itemID);
        setValue('displayName', template.displayName);
        setValue('short-description', template.thumbnailDescription);
        setValue('description', template.description);
        setValue('itemType', template.itemType || 'Misc');
        setValue('rarity', template.rarity || 'Common');
        setValue('maxStackSize', template.maxStackSize || 1);
        setValue('weight', template.weight || 0);
        setValue('value', template.value || 0);
        setValue('durability', template.durability || 100);

        setValue('itemSubCategory', template.itemSubCategory);
        setValue('maxQuantity', template.maxQuantity || 0);
        setChecked('bHasWeight', template.bHasWeight);
        setChecked('bHasPrice', template.bHasPrice);
        setChecked('bHasDurability', template.bHasDurability);
        setValue('basePrice', template.basePrice || 0);
        setValue('sellPriceCoefficient', template.sellPriceCoefficient || 0.5);
        setValue('maxDurability', template.maxDurability || 100);
        setValue('baseDurability', template.baseDurability || 100);
        setValue('durabilityPenalization', template.durabilityPenalization || 1.0);
        setValue('durabilityToPriceCoefficient', template.durabilityToPriceCoefficient || 1.0);

        setChecked('isStackable', template.isStackable);
        setChecked('isDroppable', template.isDroppable);
        setChecked('isUsable', template.isUsable);
        setChecked('isEquippable', template.isEquippable);
        setChecked('isTradeable', template.isTradeable);
        setChecked('isQuestItem', template.isQuestItem);

        setValue('iconPath', '');
        setValue('meshPath', '');

        this.clearFileInfoDisplays();
        await this.showFileInfo(template, 'icon');
        await this.showFileInfo(template, 'mesh');

        setValue('equipSlot', template.equipSlot || 'None');

        this.loadCustomProperties(template.customProperties || []);
        this.loadMaterials(template.materials || []);

        this.editor.selectedTags = template.tags || [];
        this.renderTags();

        this.editor.form.updateSubcategories();
        setValue('itemSubCategory', template.itemSubCategory);

        const equipmentSection = document.getElementById('equipmentSection');
        if (equipmentSection) {
            equipmentSection.style.display = template.isEquippable ? 'block' : 'none';
        }

        const bHasWeightField = document.getElementById('bHasWeight');
        const bHasPriceField = document.getElementById('bHasPrice');
        const bHasDurabilityField = document.getElementById('bHasDurability');

        this.editor.form.toggleSection(bHasWeightField, 'weightSection', ['weight']);
        this.editor.form.toggleSection(bHasPriceField, 'priceSection', ['basePrice', 'sellPriceCoefficient']);
        this.editor.form.toggleSection(bHasDurabilityField, 'durabilitySection', ['maxDurability', 'baseDurability', 'durabilityPenalization', 'durabilityToPriceCoefficient']);

        this.updatePreview();

        if (this.editor.dirtyStateManager) {
            this.editor.dirtyStateManager.setOriginalData(template);
        }
    }

    clearForm() {
        const templateForm = document.getElementById('templateForm');
        if (templateForm) {
            templateForm.reset();
        }

        const customPropsContainer = document.getElementById('customPropsContainer');
        if (customPropsContainer) {
            customPropsContainer.innerHTML = '';
        }

        this.editor.form.clearMaterials();

        const equipmentSection = document.getElementById('equipmentSection');
        if (equipmentSection) {
            equipmentSection.style.display = 'none';
        }

        this.clearFileInfoDisplays();
        this.editor.customPropCounter = 0;
        this.editor.materialCounter = 0;
        this.editor.selectedTags = [];
        this.renderTags();
        this.closePreview();
    }

    clearFileInfoDisplays() {
        const iconInput = document.getElementById('iconPath');
        if (iconInput) {
            const iconGroup = iconInput.closest('.form-group');
            const iconInfo = iconGroup?.querySelector('.file-info-display');
            if (iconInfo) iconInfo.remove();
        }

        const meshInput = document.getElementById('meshPath');
        if (meshInput) {
            const meshGroup = meshInput.closest('.form-group');
            const meshInfo = meshGroup?.querySelector('.file-info-display');
            if (meshInfo) meshInfo.remove();
        }
    }

    async showFileInfo(template, fileType) {
        const fileId = fileType === 'icon' ? template.iconFileId : template.meshFileId;
        if (!fileId) return;

        const fileData = await this.editor.fileRepo.get(fileId);
        if (!fileData) return;

        const inputId = fileType === 'icon' ? 'iconPath' : 'meshPath';
        const input = document.getElementById(inputId);
        if (!input) return;

        const fileGroup = input.closest('.form-group');
        if (!fileGroup) return;

        const existingInfo = fileGroup.querySelector('.file-info-display');
        if (existingInfo) existingInfo.remove();

        const fileInfoDiv = document.createElement('div');
        fileInfoDiv.className = 'file-info-display';

        const statusColor = 'var(--success-color)';
        const statusIcon = fileType === 'icon' ? '🖼️' : '🎨';

        fileInfoDiv.style.cssText = `
            margin-top: var(--spacing-xs);
            padding: var(--spacing-sm);
            background: var(--bg-light);
            border-radius: var(--border-radius-sm);
            border-left: 3px solid ${statusColor};
            font-size: var(--font-size-sm);
            position: relative;
        `;

        let infoText = `${statusIcon} Stored ${fileType}: ${fileData.fileName}`;
        infoText += `\n📊 Size: ${(fileData.size / 1024).toFixed(1)} KB`;
        infoText += `\n✨ Ready for export!`;

        if (fileType === 'icon' && fileData.fileType.startsWith('image/')) {
            const preview = document.createElement('img');
            const previewUrl = await this.editor.fileRepo.getPreviewUrl(fileId);
            if (previewUrl) {
                preview.src = previewUrl;
                preview.style.cssText = `
                    width: 32px;
                    height: 32px;
                    object-fit: cover;
                    border-radius: var(--border-radius-sm);
                    float: right;
                    margin-left: var(--spacing-sm);
                `;
                fileInfoDiv.appendChild(preview);
            }
        }

        fileInfoDiv.appendChild(document.createTextNode(infoText));
        fileInfoDiv.style.whiteSpace = 'pre-line';
        fileGroup.appendChild(fileInfoDiv);
    }

    loadCustomProperties(properties) {
        const container = document.getElementById('customPropsContainer');
        if (!container) return;

        container.innerHTML = '';
        this.editor.customPropCounter = 0;

        properties.forEach(prop => {
            this.addCustomProperty(prop.name, prop.value);
        });
    }

    loadMaterials(materials) {
        this.editor.form.setMaterials(materials);
    }

    addCustomProperty(name = '', value = '') {
        const container = document.getElementById('customPropsContainer');
        if (!container) return;

        const propId = this.editor.customPropCounter++;

        const propRow = document.createElement('div');
        propRow.className = 'custom-prop-row';
        propRow.innerHTML = `
            <div class="properties">
                <div class="form-group property">
                    <label>Property Name</label>
                    <input type="text" class="prop-name" value="${name}" placeholder="Property name">
                </div>
                <div class="form-group property">
                    <label>Value</label>
                    <input type="text" class="prop-value" value="${value}" placeholder="Property value">
                </div> 
            </div>
            <button type="button" class="btn btn-danger btn-small remove-prop close-small">✖</button>           
        `;

        propRow.querySelector('.remove-prop').addEventListener('click', () => {
            propRow.remove();
            this.updatePreview();
        });

        propRow.querySelectorAll('input').forEach(input => {
            input.addEventListener('input', () => this.updatePreview());
        });

        container.appendChild(propRow);
        this.updatePreview();
    }

    setupTagSystem() {
        const tagInput = document.getElementById('tagInput');
        if (!tagInput) return;

        tagInput.addEventListener('input', (e) => this.handleTagInput(e));
        tagInput.addEventListener('keydown', (e) => this.handleTagKeydown(e));
        tagInput.addEventListener('blur', () => {
            setTimeout(() => {
                const suggestionsDiv = document.getElementById('tagSuggestions');
                suggestionsDiv?.classList.add('hidden');
            }, 200);
        });
    }

    handleTagInput(e) {
        const value = e.target.value.toLowerCase();
        const suggestionsDiv = document.getElementById('tagSuggestions');
        
        if (value.length < 1) {
            suggestionsDiv.classList.add('hidden');
            return;
        }

        const matches = this.editor.settings.tagSuggestions.filter(tag => 
            tag.toLowerCase().includes(value) && !this.editor.selectedTags.includes(tag)
        );

        if (matches.length > 0) {
            suggestionsDiv.innerHTML = matches.slice(0, 10).map(tag => 
                `<div class="tag-suggestion" onclick="window.editor.ui.addTag('${tag}')">${tag}</div>`
            ).join('');
            suggestionsDiv.classList.remove('hidden');
        } else {
            suggestionsDiv.classList.add('hidden');
        }
    }

    handleTagKeydown(e) {
        if (e.key === 'Enter' || e.key === 'Tab') {
            e.preventDefault();
            const value = e.target.value.trim();
            if (value && !this.editor.selectedTags.includes(value)) {
                this.addTag(value);
            }
        }
    }

    addTag(tag) {
        if (!this.editor.selectedTags.includes(tag)) {
            this.editor.selectedTags.push(tag);
            this.renderTags();
            this.updatePreview();
        }
        document.getElementById('tagInput').value = '';
        document.getElementById('tagSuggestions').classList.add('hidden');
    }

    removeTag(tag) {
        this.editor.selectedTags = this.editor.selectedTags.filter(t => t !== tag);
        this.renderTags();
        this.updatePreview();
    }

    renderTags() {
        const container = document.getElementById('tagsContainer');
        const suggestions = document.getElementById('tagSuggestions');
        
        if (!container) return;
        
        container.innerHTML = '';
        
        this.editor.selectedTags.forEach(tag => {
            const tagElement = document.createElement('div');
            tagElement.className = 'tag';
            tagElement.innerHTML = `
                ${tag}
                <button type="button" class="btn btn-small" onclick="window.editor.ui.removeTag('${tag}')">×</button>
            `;
            container.appendChild(tagElement);
        });
        container.appendChild(suggestions);
    }

    updatePreview() {
        const previewContent = document.getElementById('previewContent');
        if (!previewContent) return;

        if (!this.editor.currentTemplate) {
            previewContent.textContent = 'Preview is empty. Select Template to update the Preview.';
            return;
        }

        const previewData = this.getFormData();
        const jsonString = JSON.stringify(previewData, null, 2);
        previewContent.textContent = jsonString;
    }

    getFormData() {
        const customProps = [];
        const customPropsContainer = document.getElementById('customPropsContainer');
        if (customPropsContainer) {
            const propRows = customPropsContainer.querySelectorAll('.custom-prop-row');
            propRows.forEach(row => {
                const name = row.querySelector('.prop-name')?.value;
                const value = row.querySelector('.prop-value')?.value;
                if (name && value) {
                    customProps.push({ name, value });
                }
            });
        }

        const materials = this.editor.form.getMaterials();

        const getValue = (id) => {
            const element = document.getElementById(id);
            return element ? element.value : '';
        };

        const getChecked = (id) => {
            const element = document.getElementById(id);
            return element ? element.checked : false;
        };

        return {
            itemName: getValue('itemName'),
            itemID: getValue('itemID'),
            displayName: getValue('displayName'),
            thumbnailDescription: getValue('short-description'),
            description: getValue('description'),
            itemType: getValue('itemType'),
            itemSubCategory: getValue('itemSubCategory'),
            rarity: getValue('rarity'),
            maxStackSize: parseInt(getValue('maxStackSize')) || 1,
            maxQuantity: parseInt(getValue('maxQuantity')) || 0,
            weight: parseFloat(getValue('weight')) || 0,
            value: parseInt(getValue('value')) || 0,
            durability: parseInt(getValue('durability')) || 100,
            tags: this.editor.selectedTags,
            bHasWeight: getChecked('bHasWeight'),
            bHasPrice: getChecked('bHasPrice'),
            bHasDurability: getChecked('bHasDurability'),
            basePrice: parseFloat(getValue('basePrice')) || 0,
            sellPriceCoefficient: parseFloat(getValue('sellPriceCoefficient')) || 0.5,
            maxDurability: parseInt(getValue('maxDurability')) || 100,
            baseDurability: parseInt(getValue('baseDurability')) || 100,
            durabilityPenalization: parseFloat(getValue('durabilityPenalization')) || 1.0,
            durabilityToPriceCoefficient: parseFloat(getValue('durabilityToPriceCoefficient')) || 1.0,
            isStackable: getChecked('isStackable'),
            isDroppable: getChecked('isDroppable'),
            isUsable: getChecked('isUsable'),
            isEquippable: getChecked('isEquippable'),
            isTradeable: getChecked('isTradeable'),
            isQuestItem: getChecked('isQuestItem'),
            iconFileId: this.editor.currentTemplate?.iconFileId || null,
            meshFileId: this.editor.currentTemplate?.meshFileId || null,
            meshPath: getValue('meshPath'),
            materials: materials,
            equipSlot: getValue('equipSlot'),
            customProperties: customProps
        };
    }

    showPreview() {
        const previewPanel = document.getElementById('previewPanel');
        if (previewPanel && !previewPanel.classList.contains('show')) {
            previewPanel.classList.add('show');
        }
    }

    closePreview() {
        const previewPanel = document.getElementById('previewPanel');
        if (previewPanel) {
            previewPanel.classList.remove('show');
        }
    }

    showHelp() {
        const helpModal = document.getElementById('helpModal');
        if (helpModal) {
            helpModal.classList.add('show');
        }
    }

    closeHelp() {
        const helpModal = document.getElementById('helpModal');
        if (helpModal) {
            helpModal.classList.remove('show');
        }
    }

    clearAllFormErrors() {
        const form = document.getElementById('templateForm');
        if (!form) return;

        const allFormGroups = form.querySelectorAll('.form-group');
        allFormGroups.forEach(group => {
            group.classList.remove('error');
            const errorDiv = group.querySelector('.field-error');
            if (errorDiv) {
                errorDiv.remove();
            }
        });
    }
}