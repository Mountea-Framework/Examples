export class DirtyStateManager {
    constructor(editor) {
        this.editor = editor;
        this.isDirty = false;
        this.originalData = null;
        this.autosaveIndicator = null;
        this.changeListeners = [];
    }

    init() {
        this.setupAutosaveIndicator();
        this.setupFormTracking();
        this.setupBeforeUnloadHandler();
    }

    setupAutosaveIndicator() {
        /*
        this.autosaveIndicator = document.getElementById('autosaveIndicator');
        if (this.autosaveIndicator == null) {
            const indicator = document.createElement('div');
            indicator.innerHTML = `
                <div class="autosave-indicator" id="autosaveIndicator">
                    <span class="autosave-icon">✓</span>
                    <span class="autosave-text">Saved</span>
                </div>
            `;
            document.body.appendChild(indicator.firstElementChild);
            this.autosaveIndicator = document.getElementById('autosaveIndicator');
        }
        */
    }

    setupFormTracking() {
        const form = document.getElementById('templateForm');
        if (!form) return;

        const trackableInputs = form.querySelectorAll('input, select, textarea');
        trackableInputs.forEach(input => {
            input.addEventListener('input', () => this.markDirty());
            input.addEventListener('change', () => this.markDirty());
        });

        const observer = new MutationObserver(() => {
            const newInputs = form.querySelectorAll('.prop-name, .prop-value, .material-name, .material-path');
            newInputs.forEach(input => {
                if (!input.hasAttribute('data-tracked')) {
                    input.setAttribute('data-tracked', 'true');
                    input.addEventListener('input', () => this.markDirty());
                }
            });
        });

        observer.observe(form, { childList: true, subtree: true });
    }

    setupBeforeUnloadHandler() {
        window.addEventListener('beforeunload', (e) => {
            if (this.isDirty) {
                e.preventDefault();
                e.returnValue = '';
            }
        });
    }

    setOriginalData(data) {
        this.originalData = JSON.stringify(data);
        this.isDirty = false;
        this.updateDirtyIndicators();
    }

    markDirty() {
        if (!this.editor.currentTemplate) return;
        
        const currentData = JSON.stringify(this.editor.ui.getFormData());
        this.isDirty = currentData !== this.originalData;
        this.updateDirtyIndicators();
    }

    markClean() {
        this.isDirty = false;
        this.originalData = JSON.stringify(this.editor.ui.getFormData());
        this.updateDirtyIndicators();
        this.showSaveIndicator();
    }

    updateDirtyIndicators() {
        if (!this.editor.currentTemplate) return;

        const templateContainer = document.querySelector(`[data-template-id="${this.editor.currentTemplate.id}"]`); //?.parentElement;
        if (templateContainer) {
            templateContainer.classList.toggle('dirty', this.isDirty);
        }

        const saveButton = document.getElementById('mobileSave');
        if (saveButton) {
            saveButton.style.background = this.isDirty ? 'var(--warning-color)' : 'var(--bg-white-solid)';
        }

        this.changeListeners.forEach(listener => listener(this.isDirty));
    }

    showSaveIndicator() {
        if (!this.autosaveIndicator) return;

        this.autosaveIndicator.classList.add('show');
        setTimeout(() => {
            this.autosaveIndicator.classList.remove('show');
        }, 2000);
    }

    addChangeListener(callback) {
        this.changeListeners.push(callback);
    }

    removeChangeListener(callback) {
        this.changeListeners = this.changeListeners.filter(cb => cb !== callback);
    }

    async checkDirtyBeforeAction(action, actionCallback) {
        if (!this.isDirty) {
            actionCallback();
            return;
        }

        this.editor.notifications.confirm(
            'You have unsaved changes. Do you want to save them first?',
            { confirmText: 'Save', cancelText: 'Discard' }
        ).then(shouldSave => {
            if (shouldSave) {
                this.editor.saveCurrentTemplate().then(() => {
                    actionCallback();
                });
            } else {
                this.isDirty = false;
                actionCallback();
            }
        });
    }

    isDirtyState() {
        return this.isDirty;
    }
}