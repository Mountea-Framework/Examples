export class LoadingManager {
    constructor() {
        this.loadingOverlay = null;
        this.progressBar = null;
        this.loadingText = null;
        this.startTime = Date.now();
        this.minLoadTime = 2000;
        this.currentProgress = 0;
        this.isComplete = false;
    }

    async show() {
        // Inject loading component
        const response = await fetch('components/loading.html');
        const html = await response.text();
        document.body.insertAdjacentHTML('afterbegin', html);
        
        this.loadingOverlay = document.getElementById('loadingOverlay');
        this.progressBar = document.getElementById('loadingProgress');
        this.loadingText = document.querySelector('.loading-text');
        
        this.startTime = Date.now();
    }

    updateProgress(progress, message = null) {
        if (this.progressBar) {
            this.currentProgress = Math.min(100, Math.max(0, progress));
            this.progressBar.style.width = `${this.currentProgress}%`;
        }
        
        if (message && this.loadingText) {
            this.loadingText.textContent = message;
        }
    }

    setMessage(message) {
        if (this.loadingText) {
            this.loadingText.textContent = message;
        }
    }

    async complete() {
        if (this.isComplete) return;
        this.isComplete = true;

        // Ensure minimum load time
        const elapsed = Date.now() - this.startTime;
        const remainingTime = Math.max(0, this.minLoadTime - elapsed);
        
        // Complete progress bar
        this.updateProgress(100, 'Ready!');
        
        if (remainingTime > 0) {
            await new Promise(resolve => setTimeout(resolve, remainingTime));
        }
        
        // Fade out
        if (this.loadingOverlay) {
            this.loadingOverlay.classList.add('fade-out');
            
            // Remove after animation
            setTimeout(() => {
                if (this.loadingOverlay && this.loadingOverlay.parentNode) {
                    this.loadingOverlay.parentNode.removeChild(this.loadingOverlay);
                }
            }, 500);
        }
    }

    // Progress tracking for different stages
    trackComponentLoading(componentIndex, totalComponents) {
        const progress = (componentIndex / totalComponents) * 30; // 0-30%
        this.updateProgress(progress, `Loading components... (${componentIndex}/${totalComponents})`);
    }

    trackDatabaseInit() {
        this.updateProgress(35, 'Initializing database...');
    }

    trackSettingsLoad() {
        this.updateProgress(50, 'Loading settings...');
    }

    trackTemplatesLoad() {
        this.updateProgress(70, 'Loading templates...');
    }

    trackUIInit() {
        this.updateProgress(85, 'Initializing interface...');
    }

    trackEventListeners() {
        this.updateProgress(95, 'Setting up event listeners...');
    }

    trackFinalizing() {
        this.updateProgress(98, 'Finalizing...');
    }
}