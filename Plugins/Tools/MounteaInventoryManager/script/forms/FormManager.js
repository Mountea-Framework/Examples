export class FormManager {
    constructor(editor) {
        this.editor = editor;
    }

    serializeForm() {
        return this.editor.ui.getFormData();
    }

    async populateForm(template) {
        await this.editor.ui.loadTemplateToForm(template);
    }

    clearForm() {
        this.editor.ui.clearForm();
    }

    getFormErrors() {
        const form = document.getElementById('templateForm');
        const errorGroups = form.querySelectorAll('.form-group.error');
        
        return Array.from(errorGroups).map(group => {
            const label = group.querySelector('label')?.textContent || 'Unknown field';
            const errorDiv = group.querySelector('.field-error');
            const message = errorDiv?.textContent || 'Invalid value';
            return { field: label, message };
        });
    }

    hasFormErrors() {
        const form = document.getElementById('templateForm');
        return form.querySelectorAll('.form-group.error').length > 0;
    }

    focusFirstError() {
        const form = document.getElementById('templateForm');
        const firstErrorGroup = form.querySelector('.form-group.error');
        
        if (firstErrorGroup) {
            const input = firstErrorGroup.querySelector('input, select, textarea');
            if (input) {
                input.focus();
                input.scrollIntoView({ behavior: 'smooth', block: 'center' });
            }
        }
    }

    setFormField(fieldId, value) {
        const field = document.getElementById(fieldId);
        if (!field) return false;

        if (field.type === 'checkbox') {
            field.checked = Boolean(value);
        } else {
            field.value = value;
        }

        field.dispatchEvent(new Event('input', { bubbles: true }));
        return true;
    }

    getFormField(fieldId) {
        const field = document.getElementById(fieldId);
        if (!field) return null;

        if (field.type === 'checkbox') {
            return field.checked;
        }
        return field.value;
    }

    resetFormField(fieldId) {
        const field = document.getElementById(fieldId);
        if (!field) return false;

        if (field.type === 'checkbox') {
            field.checked = false;
        } else if (field.type === 'number') {
            field.value = field.defaultValue || '0';
        } else {
            field.value = field.defaultValue || '';
        }

        field.dispatchEvent(new Event('input', { bubbles: true }));
        return true;
    }

    populateDropdown(selectId, options, valueKey = null, textKey = null) {
        const select = document.getElementById(selectId);
        if (!select) return false;

        select.innerHTML = '';

        options.forEach(option => {
            const optionElement = document.createElement('option');
            
            if (typeof option === 'string') {
                optionElement.value = option;
                optionElement.textContent = option;
            } else {
                optionElement.value = valueKey ? option[valueKey] : option.value;
                optionElement.textContent = textKey ? option[textKey] : option.text || option.name;
                
                if (option.color) {
                    optionElement.style.color = option.color;
                }
            }
            
            select.appendChild(optionElement);
        });

        return true;
    }

    addFormChangeListener(callback) {
        const form = document.getElementById('templateForm');
        
        const handler = (e) => {
            if (e.target.matches('input, select, textarea')) {
                callback(e.target, e);
            }
        };

        form.addEventListener('input', handler);
        form.addEventListener('change', handler);
        
        return () => {
            form.removeEventListener('input', handler);
            form.removeEventListener('change', handler);
        };
    }

    setupConditionalFields() {
        const isEquippableField = document.getElementById('isEquippable');
        const equipmentSection = document.getElementById('equipmentSection');
        
        if (isEquippableField && equipmentSection) {
            const toggleEquipmentSection = () => {
                equipmentSection.style.display = isEquippableField.checked ? 'block' : 'none';
            };

            isEquippableField.addEventListener('change', toggleEquipmentSection);
            toggleEquipmentSection();
        }

        const isStackableField = document.getElementById('isStackable');
        const maxStackSizeField = document.getElementById('maxStackSize');
        
        if (isStackableField && maxStackSizeField) {
            const handleStackable = () => {
                if (isStackableField.checked) {
                    maxStackSizeField.min = '1';
                    if (parseInt(maxStackSizeField.value) < 1) {
                        maxStackSizeField.value = '1';
                    }
                } else {
                    maxStackSizeField.value = '1';
                }
            };

            isStackableField.addEventListener('change', handleStackable);
            handleStackable();
        }

        const bHasWeightField = document.getElementById('bHasWeight');
        const bHasPriceField = document.getElementById('bHasPrice');
        const bHasDurabilityField = document.getElementById('bHasDurability');
        
        bHasWeightField?.addEventListener('change', () => this.toggleSection(bHasWeightField, 'weightSection', ['weight']));
        bHasPriceField?.addEventListener('change', () => this.toggleSection(bHasPriceField, 'priceSection', ['basePrice', 'sellPriceCoefficient']));
        bHasDurabilityField?.addEventListener('change', () => this.toggleSection(bHasDurabilityField, 'durabilitySection', ['maxDurability', 'baseDurability', 'durabilityPenalization', 'durabilityToPriceCoefficient']));
    }

    toggleSection(checkbox, sectionId, fieldIds) {
        const section = document.getElementById(sectionId);
        const isEnabled = checkbox.checked;
        section?.classList.toggle('disabled', !isEnabled);
        fieldIds.forEach(fieldId => {
            const field = document.getElementById(fieldId);
            if (field) field.disabled = !isEnabled;
        });
    }

    updateSubcategories() {
        const category = this.getFormField('itemType');
        const subcategories = this.editor.settings.subcategories[category] || [];
        const options = [{ value: '', text: 'None' }, ...subcategories.map(sub => ({ value: sub, text: sub }))];
        this.populateDropdown('itemSubCategory', options);
    }

    createFormSection(title, fields) {
        const section = document.createElement('section');
        section.className = 'form-section';

        const header = document.createElement('h2');
        header.textContent = title;
        section.appendChild(header);

        const grid = document.createElement('div');
        grid.className = 'form-grid';

        fields.forEach(field => {
            const formGroup = this.createFormGroup(field);
            grid.appendChild(formGroup);
        });

        section.appendChild(grid);
        return section;
    }

    createFormGroup(fieldConfig) {
        const group = document.createElement('div');
        group.className = 'form-group';

        const label = document.createElement('label');
        label.textContent = fieldConfig.label;
        label.setAttribute('for', fieldConfig.id);
        
        if (fieldConfig.required) {
            label.innerHTML += '<span style="color: red;">*</span>';
        }

        let input;
        
        switch (fieldConfig.type) {
            case 'select':
                input = document.createElement('select');
                if (fieldConfig.options) {
                    fieldConfig.options.forEach(option => {
                        const optElement = document.createElement('option');
                        optElement.value = option.value || option;
                        optElement.textContent = option.text || option;
                        input.appendChild(optElement);
                    });
                }
                break;
                
            case 'textarea':
                input = document.createElement('textarea');
                if (fieldConfig.rows) input.rows = fieldConfig.rows;
                break;
                
            case 'checkbox':
                input = document.createElement('input');
                input.type = 'checkbox';
                if (fieldConfig.checked) input.checked = true;
                break;
                
            default:
                input = document.createElement('input');
                input.type = fieldConfig.type || 'text';
                if (fieldConfig.min !== undefined) input.min = fieldConfig.min;
                if (fieldConfig.max !== undefined) input.max = fieldConfig.max;
                if (fieldConfig.step !== undefined) input.step = fieldConfig.step;
                if (fieldConfig.maxLength) input.maxLength = fieldConfig.maxLength;
                break;
        }

        input.id = fieldConfig.id;
        input.name = fieldConfig.name || fieldConfig.id;
        
        if (fieldConfig.required) input.required = true;
        if (fieldConfig.placeholder) input.placeholder = fieldConfig.placeholder;
        if (fieldConfig.value !== undefined) input.value = fieldConfig.value;

        group.appendChild(label);
        group.appendChild(input);

        if (fieldConfig.help) {
            const helpText = document.createElement('small');
            helpText.className = 'help-text';
            helpText.textContent = fieldConfig.help;
            group.appendChild(helpText);
        }

        return group;
    }

    getFormValues() {
        const form = document.getElementById('templateForm');
        const formData = new FormData(form);
        const values = {};

        for (const [key, value] of formData.entries()) {
            const field = form.querySelector(`[name="${key}"]`);
            
            if (field?.type === 'checkbox') {
                values[key] = field.checked;
            } else if (field?.type === 'number') {
                values[key] = value ? parseFloat(value) : 0;
            } else {
                values[key] = value;
            }
        }

        const checkboxes = form.querySelectorAll('input[type="checkbox"]');
        checkboxes.forEach(checkbox => {
            if (!values.hasOwnProperty(checkbox.name)) {
                values[checkbox.name] = false;
            }
        });

        return values;
    }

    setFormValues(values) {
        Object.entries(values).forEach(([key, value]) => {
            this.setFormField(key, value);
        });
    }

    markFieldsAsRequired(fieldIds) {
        fieldIds.forEach(fieldId => {
            const field = document.getElementById(fieldId);
            if (field) {
                field.required = true;
                const label = field.closest('.form-group')?.querySelector('label');
                if (label && !label.innerHTML.includes('*')) {
                    label.innerHTML += '<span style="color: red;">*</span>';
                }
            }
        });
    }

    removeRequiredFromFields(fieldIds) {
        fieldIds.forEach(fieldId => {
            const field = document.getElementById(fieldId);
            if (field) {
                field.required = false;
                const label = field.closest('.form-group')?.querySelector('label');
                if (label) {
                    label.innerHTML = label.innerHTML.replace(/<span[^>]*>\*<\/span>/, '');
                }
            }
        });
    }

    createCustomPropertyRow(name = '', value = '') {
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

        const removeBtn = propRow.querySelector('.remove-prop');
        removeBtn.addEventListener('click', () => {
            propRow.remove();
            this.editor.ui.updatePreview();
        });

        const inputs = propRow.querySelectorAll('input');
        inputs.forEach(input => {
            input.addEventListener('input', () => this.editor.ui.updatePreview());
        });

        return propRow;
    }

    getCustomProperties() {
        const container = document.getElementById('customPropsContainer');
        if (!container) return [];
        
        const propRows = container.querySelectorAll('.custom-prop-row');
        const properties = [];

        propRows.forEach(row => {
            const name = row.querySelector('.prop-name').value.trim();
            const value = row.querySelector('.prop-value').value.trim();
            if (name && value) {
                properties.push({ name, value });
            }
        });

        return properties;
    }

    setCustomProperties(properties) {
        const container = document.getElementById('customPropsContainer');
        if (!container) return;
        
        container.innerHTML = '';

        properties.forEach(prop => {
            const row = this.createCustomPropertyRow(prop.name, prop.value);
            container.appendChild(row);
        });
    }

    addCustomProperty(name = '', value = '') {
        const container = document.getElementById('customPropsContainer');
        if (!container) return;
        
        const row = this.createCustomPropertyRow(name, value);
        container.appendChild(row);
        this.editor.ui.updatePreview();
    }

    createMaterialRow(name = '', customPath = '') {
        const materialId = this.editor.materialCounter++;
        const materialRow = document.createElement('div');
        materialRow.className = 'material-row';
        materialRow.dataset.materialId = materialId;

        const defaultPath = name ? `/Game/Materials/${name}` : '';
        const finalPath = customPath || defaultPath;

        materialRow.innerHTML = `
            <div class="material-fields">
                <div class="form-group material-field">
                    <label>Material Name</label>
                    <input type="text" class="material-name" value="${name}" placeholder="Material name">
                </div>
                <div class="form-group material-field">
                    <label>Material Path</label>
                    <input type="text" class="material-path" value="${finalPath}" placeholder="/Game/Materials/MaterialName">
                </div>
            </div>
            <button type="button" class="btn btn-danger btn-small remove-material close-small">✖</button>
        `;

        const nameInput = materialRow.querySelector('.material-name');
        const pathInput = materialRow.querySelector('.material-path');
        const removeBtn = materialRow.querySelector('.remove-material');

        nameInput.addEventListener('input', (e) => {
            const name = e.target.value.trim();
            if (name && (!pathInput.value || pathInput.value === '' || pathInput.value.startsWith('/Game/Materials/'))) {
                pathInput.value = `/Game/Materials/${name}`;
            }
            this.editor.ui.updatePreview();
        });

        pathInput.addEventListener('input', () => {
            this.editor.ui.updatePreview();
        });

        removeBtn.addEventListener('click', () => {
            materialRow.remove();
            this.editor.ui.updatePreview();
        });

        return materialRow;
    }

    addMaterialRow(name = '', customPath = '') {
        const container = document.getElementById('materialsContainer');
        if (!container) return;

        const row = this.createMaterialRow(name, customPath);
        container.appendChild(row);
        this.editor.ui.updatePreview();
    }

    getMaterials() {
        const container = document.getElementById('materialsContainer');
        if (!container) return [];

        const materialRows = container.querySelectorAll('.material-row');
        const materials = [];

        materialRows.forEach(row => {
            const name = row.querySelector('.material-name').value.trim();
            const path = row.querySelector('.material-path').value.trim();
            if (name && path) {
                materials.push({ name, path });
            }
        });

        return materials;
    }

    setMaterials(materials) {
        const container = document.getElementById('materialsContainer');
        if (!container) return;

        container.innerHTML = '';
        this.editor.materialCounter = 0;

        materials.forEach(material => {
            this.addMaterialRow(material.name, material.path);
        });
    }

    clearMaterials() {
        const container = document.getElementById('materialsContainer');
        if (container) {
            container.innerHTML = '';
            this.editor.materialCounter = 0;
        }
    }

    populateMaterialPresets() {
        const presets = this.editor.settings.materialPresets || [];
        return presets.map(preset => ({ value: preset, text: preset }));
    }

    addMaterialFromPreset(presetName) {
        this.addMaterialRow(presetName, `/Game/Materials/${presetName}`);
    }

    validateMaterialName(name) {
        if (!name || name.trim() === '') {
            return { isValid: false, message: 'Material name is required' };
        }
        
        if (name.length > 50) {
            return { isValid: false, message: 'Material name must be 50 characters or less' };
        }
        
        if (!/^[a-zA-Z0-9_]+$/.test(name)) {
            return { isValid: false, message: 'Material name can only contain letters, numbers, and underscores' };
        }
        
        return { isValid: true };
    }

    validateMaterialPath(path) {
        if (!path || path.trim() === '') {
            return { isValid: false, message: 'Material path is required' };
        }
        
        if (path.length > 200) {
            return { isValid: false, message: 'Material path must be 200 characters or less' };
        }
        
        if (!path.startsWith('/Game/')) {
            return { isValid: false, message: 'Material path should start with /Game/' };
        }
        
        return { isValid: true };
    }

    duplicateMaterial(materialRow) {
        const nameInput = materialRow.querySelector('.material-name');
        const pathInput = materialRow.querySelector('.material-path');
        
        const originalName = nameInput.value;
        const originalPath = pathInput.value;
        
        const duplicateName = `${originalName}_Copy`;
        const duplicatePath = originalPath.replace(originalName, duplicateName);
        
        this.addMaterialRow(duplicateName, duplicatePath);
    }

    reorderMaterials(fromIndex, toIndex) {
        const container = document.getElementById('materialsContainer');
        if (!container) return;
        
        const rows = Array.from(container.querySelectorAll('.material-row'));
        if (fromIndex < 0 || fromIndex >= rows.length || toIndex < 0 || toIndex >= rows.length) {
            return;
        }
        
        const movedRow = rows[fromIndex];
        
        if (fromIndex < toIndex) {
            container.insertBefore(movedRow, rows[toIndex].nextSibling);
        } else {
            container.insertBefore(movedRow, rows[toIndex]);
        }
        
        this.editor.ui.updatePreview();
    }

    getMaterialByName(name) {
        const materials = this.getMaterials();
        return materials.find(material => material.name === name);
    }

    hasDuplicateMaterialNames() {
        const materials = this.getMaterials();
        const names = materials.map(m => m.name.toLowerCase());
        return names.length !== new Set(names).size;
    }

    getMaterialCount() {
        return this.getMaterials().length;
    }

    exportMaterialsOnly() {
        const materials = this.getMaterials();
        return {
            version: '1.0.0',
            exportDate: new Date().toISOString(),
            materials: materials
        };
    }

    importMaterialsOnly(data) {
        if (!data.materials || !Array.isArray(data.materials)) {
            throw new Error('Invalid materials data format');
        }
        
        this.clearMaterials();
        data.materials.forEach(material => {
            this.addMaterialRow(material.name, material.path);
        });
    }
}