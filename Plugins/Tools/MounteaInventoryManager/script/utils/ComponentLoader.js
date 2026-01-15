export class ComponentLoader {
    constructor() {
        this.cache = new Map();
        this.loadedComponents = new Set();
    }

    async loadComponent(path, targetSelector = null) {
        try {
            // Check cache first
            if (this.cache.has(path)) {
                return this.cache.get(path);
            }

            const response = await fetch(`components/${path}`);
            if (!response.ok) {
                throw new Error(`Failed to load component: ${path}`);
            }

            const html = await response.text();
            this.cache.set(path, html);

            // If target selector provided, inject immediately
            if (targetSelector) {
                const target = document.querySelector(targetSelector);
                if (target) {
                    target.innerHTML = html;
                    this.loadedComponents.add(path);
                }
            }

            return html;
        } catch (error) {
            console.error(`Component loading error for ${path}:`, error);
            return '';
        }
    }

    async loadComponents(components) {
        const loadPromises = components.map(({ path, target }) => 
            this.loadComponent(path, target)
        );
        
        await Promise.all(loadPromises);
    }

    async injectComponent(path, targetSelector) {
        const html = await this.loadComponent(path);
        const target = document.querySelector(targetSelector);
        
        if (target && html) {
            target.innerHTML = html;
            this.loadedComponents.add(path);
        }
    }

    isLoaded(path) {
        return this.loadedComponents.has(path);
    }

    clearCache() {
        this.cache.clear();
        this.loadedComponents.clear();
    }
}