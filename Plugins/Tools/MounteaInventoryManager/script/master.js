import { InventoryEditor } from './core/InventoryEditor.js';

let editor = null;

// Initialize the editor when DOM is loaded
document.addEventListener('DOMContentLoaded', async () => {
    try {
        editor = new InventoryEditor();
        await editor.initPromise;
        adjustMainContentPadding();
        window.editor = editor;
    } catch (error) {
        console.error('Failed to initialize application:', error);
    }
});

// Prevent right-click context menu
document.addEventListener('contextmenu', (event) => {
    event.preventDefault();
});

// Adjust main content padding for responsive design
function adjustMainContentPadding() {
    const header = document.querySelector('header');
    const mainContent = document.querySelector('.main-content');
    
    if (header && mainContent) {
        const headerHeight = header.offsetHeight;
        mainContent.style.paddingTop = `${headerHeight}px`;
    }
}

// Only adjust padding after components are loaded
window.addEventListener('load', () => {
    if (editor && editor.componentsLoaded) {
        adjustMainContentPadding();
    }
});

window.addEventListener('resize', adjustMainContentPadding);