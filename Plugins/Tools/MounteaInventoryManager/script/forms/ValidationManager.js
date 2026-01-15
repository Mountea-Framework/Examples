export class ValidationManager {
    constructor(editor) {
        this.editor = editor;
        this.validationRules = {
            itemName: {
                required: true,
                minLength: 1,
                maxLength: 100,
                message: 'Item name is required and must be 1-100 characters'
            },
            itemID: {
                required: true,
                pattern: /^[A-Za-z0-9_-]+$/,
                message: 'Item ID is required and can only contain letters, numbers, underscores, and hyphens'
            },
            displayName: {
                required: true,
                minLength: 1,
                maxLength: 100,
                message: 'Display name is required and must be 1-100 characters'
            },
            maxStackSize: {
                required: true,
                min: 1,
                max: 9999,
                type: 'number',
                message: 'Max stack size must be between 1 and 9999'
            },
            weight: {
                min: 0,
                max: 999999,
                type: 'number',
                message: 'Weight must be between 0 and 999999'
            },
            value: {
                min: 0,
                max: 999999999,
                type: 'number',
                message: 'Value must be between 0 and 999999999'
            },
            durability: {
                min: 0,
                max: 999999,
                type: 'number',
                message: 'Durability must be between 0 and 999999'
            },
            basePrice: {
                min: 0,
                max: 999999999,
                type: 'number',
                message: 'Base price must be between 0 and 999999999'
            },
            sellPriceCoefficient: {
                min: 0,
                max: 10,
                type: 'number',
                message: 'Sell price coefficient must be between 0 and 10'
            },
            maxDurability: {
                min: 1,
                max: 999999,
                type: 'number',
                message: 'Max durability must be between 1 and 999999'
            },
            baseDurability: {
                min: 0,
                max: 999999,
                type: 'number',
                message: 'Base durability must be between 0 and 999999'
            },
            durabilityPenalization: {
                min: 0,
                max: 100,
                type: 'number',
                message: 'Durability penalization must be between 0 and 100'
            },
            durabilityToPriceCoefficient: {
                min: 0,
                max: 10,
                type: 'number',
                message: 'Durability to price coefficient must be between 0 and 10'
            }
        };
    }

    setupFormValidation() {
        const form = document.getElementById('templateForm');
        if (!form) return;

        const inputs = form.querySelectorAll('input, select, textarea');
        inputs.forEach(input => {
            input.addEventListener('blur', () => this.validateField(input));
            input.addEventListener('input', () => this.clearFieldError(input.id));
        });

        form.addEventListener('submit', (e) => {
            e.preventDefault();
            this.validateForm();
        });
    }

    validateForm() {
        const form = document.getElementById('templateForm');
        if (!form) return { isValid: false, errors: [] };

        let isValid = true;
        const errors = [];

        this.clearAllErrors();

        Object.keys(this.validationRules).forEach(fieldName => {
            const field = document.getElementById(fieldName);
            if (field) {
                const fieldResult = this.validateField(field);
                if (!fieldResult.isValid) {
                    isValid = false;
                    errors.push({
                        field: fieldName,
                        message: fieldResult.message
                    });
                }
            }
        });

        const materialErrors = this.validateMaterials();
        if (materialErrors.length > 0) {
            isValid = false;
            errors.push(...materialErrors);
        }

        const customPropErrors = this.validateCustomProperties();
        if (customPropErrors.length > 0) {
            isValid = false;
            errors.push(...customPropErrors);
        }

        if (!isValid) {
            this.editor.form.focusFirstError();
        }

        return { isValid, errors };
    }

    validateField(field) {
        const fieldName = field.id;
        const rules = this.validationRules[fieldName];
        
        if (!rules) {
            return { isValid: true };
        }

        const value = field.type === 'checkbox' ? field.checked : field.value;
        const errors = [];

        if (rules.required && (!value || (typeof value === 'string' && value.trim() === ''))) {
            errors.push('This field is required');
        }

        if (value && typeof value === 'string') {
            if (rules.minLength && value.length < rules.minLength) {
                errors.push(`Must be at least ${rules.minLength} characters`);
            }

            if (rules.maxLength && value.length > rules.maxLength) {
                errors.push(`Must be no more than ${rules.maxLength} characters`);
            }

            if (rules.pattern && !rules.pattern.test(value)) {
                errors.push(rules.message || 'Invalid format');
            }
        }

        if (rules.type === 'number') {
            const numValue = parseFloat(value);
            
            if (value !== '' && (isNaN(numValue) || !isFinite(numValue))) {
                errors.push('Must be a valid number');
            } else if (!isNaN(numValue)) {
                if (rules.min !== undefined && numValue < rules.min) {
                    errors.push(`Must be at least ${rules.min}`);
                }

                if (rules.max !== undefined && numValue > rules.max) {
                    errors.push(`Must be no more than ${rules.max}`);
                }
            }
        }

        const isValid = errors.length === 0;
        
        if (!isValid) {
            this.showFieldError(field, errors[0]);
        } else {
            this.clearFieldError(fieldName);
        }

        return { 
            isValid, 
            message: errors[0] || (rules.message || 'Invalid value')
        };
    }

    validateMaterials() {
        const errors = [];
        const container = document.getElementById('materialsContainer');
        
        if (!container) return errors;

        const materialRows = container.querySelectorAll('.material-row');
        
        materialRows.forEach((row, index) => {
            const nameInput = row.querySelector('.material-name');
            const pathInput = row.querySelector('.material-path');
            
            if (!nameInput || !pathInput) return;

            const name = nameInput.value.trim();
            const path = pathInput.value.trim();

            this.clearMaterialRowErrors(row);

            if (!name && !path) {
                return;
            }

            if (!name) {
                this.showMaterialFieldError(nameInput, 'Material name is required');
                errors.push({
                    field: `materials[${index}].name`,
                    message: 'Material name is required'
                });
            } else if (name.length > 50) {
                this.showMaterialFieldError(nameInput, 'Material name must be no more than 50 characters');
                errors.push({
                    field: `materials[${index}].name`,
                    message: 'Material name must be no more than 50 characters'
                });
            }

            if (!path) {
                this.showMaterialFieldError(pathInput, 'Material path is required');
                errors.push({
                    field: `materials[${index}].path`,
                    message: 'Material path is required'
                });
            } else if (path.length > 200) {
                this.showMaterialFieldError(pathInput, 'Material path must be no more than 200 characters');
                errors.push({
                    field: `materials[${index}].path`,
                    message: 'Material path must be no more than 200 characters'
                });
            } else if (!path.startsWith('/Game/')) {
                this.showMaterialFieldError(pathInput, 'Material path should start with /Game/');
                errors.push({
                    field: `materials[${index}].path`,
                    message: 'Material path should start with /Game/'
                });
            }
        });

        const materialNames = [];
        materialRows.forEach((row, index) => {
            const nameInput = row.querySelector('.material-name');
            const name = nameInput?.value.trim();
            
            if (name) {
                if (materialNames.includes(name)) {
                    this.showMaterialFieldError(nameInput, 'Material names must be unique');
                    errors.push({
                        field: `materials[${index}].name`,
                        message: 'Material names must be unique'
                    });
                } else {
                    materialNames.push(name);
                }
            }
        });

        return errors;
    }

    validateCustomProperties() {
        const errors = [];
        const container = document.getElementById('customPropsContainer');
        
        if (!container) return errors;

        const propRows = container.querySelectorAll('.custom-prop-row');
        
        propRows.forEach((row, index) => {
            const nameInput = row.querySelector('.prop-name');
            const valueInput = row.querySelector('.prop-value');
            
            if (!nameInput || !valueInput) return;

            const name = nameInput.value.trim();
            const value = valueInput.value.trim();

            this.clearCustomPropRowErrors(row);

            if (!name && !value) {
                return;
            }

            if (!name) {
                this.showCustomPropFieldError(nameInput, 'Property name is required');
                errors.push({
                    field: `customProperties[${index}].name`,
                    message: 'Property name is required'
                });
            } else if (name.length > 50) {
                this.showCustomPropFieldError(nameInput, 'Property name must be no more than 50 characters');
                errors.push({
                    field: `customProperties[${index}].name`,
                    message: 'Property name must be no more than 50 characters'
                });
            }

            if (!value) {
                this.showCustomPropFieldError(valueInput, 'Property value is required');
                errors.push({
                    field: `customProperties[${index}].value`,
                    message: 'Property value is required'
                });
            } else if (value.length > 200) {
                this.showCustomPropFieldError(valueInput, 'Property value must be no more than 200 characters');
                errors.push({
                    field: `customProperties[${index}].value`,
                    message: 'Property value must be no more than 200 characters'
                });
            }
        });

        const propNames = [];
        propRows.forEach((row, index) => {
            const nameInput = row.querySelector('.prop-name');
            const name = nameInput?.value.trim();
            
            if (name) {
                if (propNames.includes(name)) {
                    this.showCustomPropFieldError(nameInput, 'Property names must be unique');
                    errors.push({
                        field: `customProperties[${index}].name`,
                        message: 'Property names must be unique'
                    });
                } else {
                    propNames.push(name);
                }
            }
        });

        return errors;
    }

    showFieldError(field, message) {
        const formGroup = field.closest('.form-group');
        if (!formGroup) return;

        formGroup.classList.add('error');
        
        let errorDiv = formGroup.querySelector('.field-error');
        if (!errorDiv) {
            errorDiv = document.createElement('div');
            errorDiv.className = 'field-error';
            formGroup.appendChild(errorDiv);
        }
        
        errorDiv.textContent = message;
    }

    showMaterialFieldError(field, message) {
        const materialField = field.closest('.material-field');
        if (!materialField) return;

        materialField.classList.add('error');
        
        let errorDiv = materialField.querySelector('.field-error');
        if (!errorDiv) {
            errorDiv = document.createElement('div');
            errorDiv.className = 'field-error';
            materialField.appendChild(errorDiv);
        }
        
        errorDiv.textContent = message;
    }

    showCustomPropFieldError(field, message) {
        const propField = field.closest('.form-group');
        if (!propField) return;

        propField.classList.add('error');
        
        let errorDiv = propField.querySelector('.field-error');
        if (!errorDiv) {
            errorDiv = document.createElement('div');
            errorDiv.className = 'field-error';
            propField.appendChild(errorDiv);
        }
        
        errorDiv.textContent = message;
    }

    clearFieldError(fieldId) {
        const field = document.getElementById(fieldId);
        if (!field) return;

        const formGroup = field.closest('.form-group');
        if (formGroup) {
            formGroup.classList.remove('error');
            const errorDiv = formGroup.querySelector('.field-error');
            if (errorDiv) {
                errorDiv.remove();
            }
        }
    }

    clearMaterialRowErrors(row) {
        const materialFields = row.querySelectorAll('.material-field');
        materialFields.forEach(field => {
            field.classList.remove('error');
            const errorDiv = field.querySelector('.field-error');
            if (errorDiv) {
                errorDiv.remove();
            }
        });
    }

    clearCustomPropRowErrors(row) {
        const propFields = row.querySelectorAll('.form-group');
        propFields.forEach(field => {
            field.classList.remove('error');
            const errorDiv = field.querySelector('.field-error');
            if (errorDiv) {
                errorDiv.remove();
            }
        });
    }

    clearAllErrors() {
        const form = document.getElementById('templateForm');
        if (!form) return;

        const errorGroups = form.querySelectorAll('.form-group.error, .material-field.error');
        errorGroups.forEach(group => {
            group.classList.remove('error');
            const errorDiv = group.querySelector('.field-error');
            if (errorDiv) {
                errorDiv.remove();
            }
        });
    }

    isFormValid() {
        return this.validateForm().isValid;
    }

    getValidationErrors() {
        return this.validateForm().errors;
    }

    validateOnSubmit(callback) {
        const form = document.getElementById('templateForm');
        if (!form) return;

        form.addEventListener('submit', (e) => {
            e.preventDefault();
            const result = this.validateForm();
            callback(result);
        });
    }

    setCustomValidationRule(fieldId, rule) {
        this.validationRules[fieldId] = rule;
    }

    removeValidationRule(fieldId) {
        delete this.validationRules[fieldId];
    }
}