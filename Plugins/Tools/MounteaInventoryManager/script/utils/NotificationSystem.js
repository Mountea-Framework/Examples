export class NotificationSystem {
    constructor() {
        this.notifications = new Map();
        this.notificationId = 0;
        this.setupContainer();
    }

    setupContainer() {
        this.container = document.createElement('div');
        this.container.id = 'notification-container';
        this.container.style.cssText = `
            position: fixed;
            top: 20px;
            right: 20px;
            z-index: 10000;
            max-width: 400px;
            pointer-events: none;
            color: white !important;
        `;
        document.body.appendChild(this.container);
    }

    show(message, type = 'info', duration = 3000) {
        const id = ++this.notificationId;
        const notification = this.createNotification(message, type, id);
        
        this.notifications.set(id, notification);
        this.container.appendChild(notification.element);

        // Animate in
        requestAnimationFrame(() => {
            notification.element.style.transform = 'translateX(0)';
            notification.element.style.opacity = '1';
        });

        // Auto-dismiss
        if (duration > 0) {
            notification.timeout = setTimeout(() => {
                this.dismiss(id);
            }, duration);
        }

        return id;
    }

    createNotification(message, type, id) {
        const element = document.createElement('div');
        element.className = `notification notification-${type}`;
        element.style.cssText = `
            background: ${this.getTypeColor(type)};
            color: white;
            padding: 1rem 1.5rem;
            margin-bottom: 0.5rem;
            border-radius: 6px;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
            transform: translateX(100%);
            opacity: 0;
            transition: all 0.3s ease;
            pointer-events: auto;
            cursor: pointer;
            position: relative;
            overflow: hidden;
            color: white !important;
        `;

        const content = document.createElement('div');
        content.style.cssText = `
            display: flex;
            align-items: center;
            gap: 0.5rem;
            color: white !important;
        `;

        const icon = document.createElement('span');
        icon.textContent = this.getTypeIcon(type);
        icon.style.fontSize = '1.2rem';

        const text = document.createElement('span');
        text.textContent = message;
        text.style.flex = '1';

        const closeBtn = document.createElement('button');
        closeBtn.innerHTML = '×';
        closeBtn.style.cssText = `
            background: none;
            border: none;
            color: white !important;
            font-size: 1.5rem;
            cursor: pointer;
            padding: 0;
            margin-left: 0.5rem;
            opacity: 0.7;
        `;

        content.appendChild(icon);
        content.appendChild(text);
        content.appendChild(closeBtn);
        element.appendChild(content);

        // Click handlers
        closeBtn.addEventListener('click', (e) => {
            e.stopPropagation();
            this.dismiss(id);
        });

        element.addEventListener('click', () => this.dismiss(id));

        return { element, timeout: null };
    }

    dismiss(id) {
        const notification = this.notifications.get(id);
        if (!notification) return;

        if (notification.timeout) {
            clearTimeout(notification.timeout);
        }

        // Animate out
        notification.element.style.transform = 'translateX(100%)';
        notification.element.style.opacity = '0';

        setTimeout(() => {
            if (notification.element.parentNode) {
                notification.element.parentNode.removeChild(notification.element);
            }
            this.notifications.delete(id);
        }, 300);
    }

    dismissAll() {
        for (const id of this.notifications.keys()) {
            this.dismiss(id);
        }
    }

    getTypeColor(type) {
        const colors = {
            success: '#28a745',
            error: '#dc3545',
            warning: '#ffc107',
            info: '#17a2b8'
        };
        return colors[type] || colors.info;
    }

    getTypeIcon(type) {
        const icons = {
            success: '✓',
            error: '✗',
            warning: '⚠',
            info: 'ℹ'
        };
        return icons[type] || icons.info;
    }

    success(message, duration = 3000) {
        return this.show(message, 'success', duration);
    }

    error(message, duration = 5000) {
        return this.show(message, 'error', duration);
    }

    warning(message, duration = 4000) {
        return this.show(message, 'warning', duration);
    }

    info(message, duration = 3000) {
        return this.show(message, 'info', duration);
    }

    showProgress(message, steps = []) {
        const id = ++this.notificationId;
        const element = document.createElement('div');
        element.className = 'notification notification-progress';
        element.style.cssText = `
            background: var(--primary-color);
            color: white !important;
            padding: 1rem 1.5rem;
            margin-bottom: 0.5rem;
            border-radius: 6px;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
            transform: translateX(100%);
            opacity: 0;
            transition: all 0.3s ease;
            pointer-events: auto;
            min-width: 300px;
        `;

        const messageEl = document.createElement('div');
        messageEl.textContent = message;
        messageEl.style.marginBottom = '0.5rem';

        const progressBar = document.createElement('div');
        progressBar.style.cssText = `
            background: rgba(255, 255, 255, 0.3);
            height: 4px;
            border-radius: 2px;
            overflow: hidden;
            color: var(--text-primary);
        `;

        const progressFill = document.createElement('div');
        progressFill.style.cssText = `
            background: white;
            height: 100%;
            width: 0%;
            transition: width 0.3s ease;
            color: var(--text-primary);
        `;

        progressBar.appendChild(progressFill);
        element.appendChild(messageEl);
        element.appendChild(progressBar);

        this.container.appendChild(element);

        requestAnimationFrame(() => {
            element.style.transform = 'translateX(0)';
            element.style.opacity = '1';
        });

        const controller = {
            update: (percent, stepMessage = '') => {
                progressFill.style.width = `${Math.min(100, Math.max(0, percent))}%`;
                if (stepMessage) {
                    messageEl.textContent = stepMessage;
                }
            },
            complete: (finalMessage = 'Complete!') => {
                progressFill.style.width = '100%';
                messageEl.textContent = finalMessage;
                setTimeout(() => {
                    element.style.transform = 'translateX(100%)';
                    element.style.opacity = '0';
                    setTimeout(() => {
                        if (element.parentNode) {
                            element.parentNode.removeChild(element);
                        }
                    }, 300);
                }, 1000);
            }
        };

        return controller;
    }

    confirm(message, options = {}) {
        return new Promise((resolve) => {
            const modal = document.createElement('div');
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

            const dialog = document.createElement('div');
            dialog.style.cssText = `
                background: var(--bg-white-solid);
                padding: 2rem;
                border-radius: 8px;
                max-width: 400px;
                width: 90%;
                text-align: center;
                color: var(--text-primary);
            `;

            dialog.innerHTML = `
                <p style="margin-bottom: 1.5rem; font-size: 1.1rem;">${message}</p>
                <div style="display: flex; gap: 1rem; justify-content: center;">
                    <button id="confirmYes" class="btn btn-primary">${options.confirmText || 'Yes'}</button>
                    <button id="confirmNo" class="btn btn-secondary">${options.cancelText || 'No'}</button>
                </div>
            `;

            modal.appendChild(dialog);
            document.body.appendChild(modal);

            dialog.querySelector('#confirmYes').addEventListener('click', () => {
                document.body.removeChild(modal);
                resolve(true);
            });

            dialog.querySelector('#confirmNo').addEventListener('click', () => {
                document.body.removeChild(modal);
                resolve(false);
            });

            modal.addEventListener('click', (e) => {
                if (e.target === modal) {
                    document.body.removeChild(modal);
                    resolve(false);
                }
            });
        });
    }
}