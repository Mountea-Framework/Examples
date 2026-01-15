export class FileHandler {
    constructor(editor) {
        this.editor = editor;
        this.setupFileInputListeners();
    }

    setupFileInputListeners() {
        const iconInput = document.getElementById('iconPath');
        const meshInput = document.getElementById('meshPath');

        if (iconInput) {
            iconInput.addEventListener('change', (e) => this.handleFileSelection(e, 'icon'));
        }

        if (meshInput) {
            meshInput.addEventListener('change', (e) => this.handleFileSelection(e, 'mesh'));
        }
    }

    async handleFileSelection(event, fileType) {
        const file = event.target.files[0];
        if (!file) return;

        try {
            const processedFile = await this.editor.fileRepo.processFile(file);
            
            if (!processedFile) {
                throw new Error('Failed to process file');
            }

            const fileId = await this.editor.fileRepo.save(processedFile);
            
            if (!fileId) {
                throw new Error('Failed to save file to database');
            }

            if (this.editor.currentTemplate) {
                if (fileType === 'icon') {
                    this.editor.currentTemplate.iconFileId = fileId;
                } else if (fileType === 'mesh') {
                    this.editor.currentTemplate.meshFileId = fileId;
                    await this.extractMaterialsFromMesh(file);
                }

                // Refresh only materials UI without saving
                if (fileType === 'mesh') {
                    this.editor.form.clearMaterials();
                    this.editor.form.setMaterials(this.editor.currentTemplate.materials);
                }
                
                await this.editor.templateRepo.save(this.editor.currentTemplate);
                await this.editor.ui.showFileInfo(this.editor.currentTemplate, fileType);
            }

            this.editor.showNotification(`${fileType} file uploaded successfully`, 'success');

        } catch (error) {
            console.error('File storage error:', error);
            const message = error.message || 'Failed to upload file';
            this.editor.showNotification(message, 'error');
        } finally {
            event.target.value = '';
        }
    }

    async removeFile(fileType) {
        if (!this.editor.currentTemplate) {
            this.editor.showNotification('No template selected', 'error');
            return;
        }

        try {
            const fileIdKey = fileType === 'icon' ? 'iconFileId' : 'meshFileId';
            const fileId = this.editor.currentTemplate[fileIdKey];

            if (fileId) {
                await this.editor.fileRepo.delete(fileId);
                this.editor.currentTemplate[fileIdKey] = null;
                await this.editor.templateRepo.save(this.editor.currentTemplate);
            }

            this.clearFileDisplay(fileType);
            this.editor.showNotification(`${fileType} file removed`, 'success');

        } catch (error) {
            console.error('File removal error:', error);
            const message = error.message || 'Failed to remove file';
            this.editor.showNotification(message, 'error');
        }
    }

    clearFileDisplay(fileType) {
        const inputId = fileType === 'icon' ? 'iconPath' : 'meshPath';
        const input = document.getElementById(inputId);
        
        if (input) {
            input.value = '';
            const fileGroup = input.closest('.form-group');
            const fileInfo = fileGroup?.querySelector('.file-info-display');
            if (fileInfo) {
                fileInfo.remove();
            }
        }
    }

    async downloadFile(fileId) {
        try {
            const success = await this.editor.fileRepo.downloadFile(fileId);
            if (success) {
                this.editor.showNotification('File downloaded successfully', 'success');
            } else {
                this.editor.showNotification('Failed to download file', 'error');
            }
        } catch (error) {
            console.error('File download error:', error);
            const message = error.message || 'Failed to download file';
            this.editor.showNotification(message, 'error');
        }
    }

    validateFileType(file, allowedTypes) {
        const fileExtension = file.name.toLowerCase().split('.').pop();
        const isValidType = allowedTypes.includes(fileExtension) || 
                           allowedTypes.includes(file.type);
        
        if (!isValidType) {
            throw new Error(`File type not supported. Allowed types: ${allowedTypes.join(', ')}`);
        }
        
        return true;
    }

    validateFileSize(file, maxSizeMB = 50) {
        const maxSize = maxSizeMB * 1024 * 1024;
        
        if (file.size > maxSize) {
            throw new Error(`File size exceeds ${maxSizeMB}MB limit`);
        }
        
        return true;
    }

    getFileTypeFromInput(inputId) {
        switch (inputId) {
            case 'iconPath':
                return 'icon';
            case 'meshPath':
                return 'mesh';
            default:
                return 'unknown';
        }
    }

    async getFileStats() {
        try {
            return await this.editor.fileRepo.getStats();
        } catch (error) {
            console.error('Failed to get file stats:', error);
            return { total: 0, totalSize: 0, byType: {}, averageSize: 0 };
        }
    }

    async cleanupOldFiles() {
        try {
            const removedCount = await this.editor.fileRepo.cleanup();
            if (removedCount > 0) {
                this.editor.showNotification(`Cleaned up ${removedCount} old files`, 'success');
            }
            return removedCount;
        } catch (error) {
            console.error('Cleanup error:', error);
            const message = error.message || 'Failed to cleanup old files';
            this.editor.showNotification(message, 'error');
            return 0;
        }
    }

    formatFileSize(bytes) {
        if (bytes === 0) return '0 Bytes';
        
        const k = 1024;
        const sizes = ['Bytes', 'KB', 'MB', 'GB'];
        const i = Math.floor(Math.log(bytes) / Math.log(k));
        
        return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
    }

    isImageFile(file) {
        return file.type.startsWith('image/');
    }

    isMeshFile(file) {
        const meshExtensions = ['.fbx', '.obj'];
        const extension = '.' + file.name.toLowerCase().split('.').pop();
        return meshExtensions.includes(extension);
    }

    async extractMaterialsFromMesh(file) {
        try {
            const fileName = file.name.toLowerCase();
            const baseName = fileName.split('.')[0];
            
            // Clear existing materials
            this.editor.currentTemplate.materials = [];
            
            // Auto-generate material based on mesh name
            const materialName = this.generateMaterialName(baseName);
            const materialPath = `/Game/Materials/${materialName}`;
            
            this.editor.currentTemplate.materials.push({
                name: materialName,
                path: materialPath
            });
            
            // Update UI
            if (this.editor.materialsManager) {
                this.editor.materialsManager.renderMaterials();
            }
            
        } catch (error) {
            console.error('Failed to extract materials:', error);
        }
    }

    generateMaterialName(baseName) {
        // Convert mesh name to material name
        // Remove common prefixes/suffixes
        let materialName = baseName
            .replace(/^(sm_|static_|mesh_)/i, '')
            .replace(/(_lod\d+|_mesh|_static)$/i, '');
        
        // Capitalize first letter of each word
        materialName = materialName
            .split(/[_\s-]+/)
            .map(word => word.charAt(0).toUpperCase() + word.slice(1).toLowerCase())
            .join('_');
        
        return `M_${materialName}` || 'M_Material';
    }
}