export class ThemeManager {
    constructor(editor) {
        this.editor = editor;
        this.currentTheme = 'light';
        this.init();
    }

    init() {
        // Load saved theme or detect system preference
        const savedTheme = localStorage.getItem('theme');
        const systemPrefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
        
        this.currentTheme = savedTheme || (systemPrefersDark ? 'dark' : 'light');
        this.applyTheme(this.currentTheme);
        
        // Listen for system theme changes
        window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', (e) => {
            if (!localStorage.getItem('theme')) {
                this.setTheme(e.matches ? 'dark' : 'light');
            }
        });
        
        this.createThemeToggle();
    }

    createThemeToggle() {
        const themeToggle = document.createElement('button');
        themeToggle.id = 'themeToggle';
        themeToggle.className = 'btn btn-secondary';
        themeToggle.innerHTML = this.getThemeIcon();
        themeToggle.setAttribute('aria-label', 'Toggle dark mode');
        themeToggle.title = 'Toggle dark/light theme';
        
        themeToggle.addEventListener('click', () => this.toggleTheme());
        
        // Add to header controls
        const headerControls = document.querySelector('.header-controls');
        if (headerControls) {
            const listItem = document.createElement('li');
            listItem.appendChild(themeToggle);
            headerControls.appendChild(listItem);
        }
    }

    getThemeIcon() {
        return this.currentTheme === 'dark' ? 
            '☀️' : // Sun icon for light mode when in dark theme
            '🌙';  // Moon icon for dark mode when in light theme
    }

    toggleTheme() {
        const newTheme = this.currentTheme === 'dark' ? 'light' : 'dark';
        this.setTheme(newTheme);
    }

    setTheme(theme) {
        this.currentTheme = theme;
        this.applyTheme(theme);
        this.saveTheme(theme);
        this.updateToggleIcon();
    }

    applyTheme(theme) {
        document.documentElement.setAttribute('data-theme', theme);
        
        // Update meta theme-color for mobile browsers
        let metaTheme = document.querySelector('meta[name="theme-color"]');
        if (!metaTheme) {
            metaTheme = document.createElement('meta');
            metaTheme.name = 'theme-color';
            document.head.appendChild(metaTheme);
        }
        
        metaTheme.content = theme === 'dark' ? '#1e293b' : '#ffffff';
    }

    updateToggleIcon() {
        const toggleBtn = document.getElementById('themeToggle');
        if (toggleBtn) {
            toggleBtn.innerHTML = this.getThemeIcon();
            toggleBtn.title = `Switch to ${this.currentTheme === 'dark' ? 'light' : 'dark'} theme`;
        }
    }

    saveTheme(theme) {
        localStorage.setItem('theme', theme);
    }

    getCurrentTheme() {
        return this.currentTheme;
    }

    isDarkMode() {
        return this.currentTheme === 'dark';
    }

    // Method to programmatically set theme (for settings import/export)
    setThemeFromSettings(theme) {
        if (theme === 'dark' || theme === 'light') {
            this.setTheme(theme);
        }
    }

    // Export theme preference for settings backup
    exportThemeSettings() {
        return {
            theme: this.currentTheme,
            autoDetect: !localStorage.getItem('theme')
        };
    }

    // Import theme settings
    importThemeSettings(settings) {
        if (settings.autoDetect) {
            localStorage.removeItem('theme');
            const systemPrefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
            this.setTheme(systemPrefersDark ? 'dark' : 'light');
        } else if (settings.theme) {
            this.setTheme(settings.theme);
        }
    }
}