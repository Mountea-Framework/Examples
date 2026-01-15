export const AppConfig = {
    // Database Configuration
    database: {
        name: 'InventoryTemplateDB',
        version: 2,
        stores: {
            templates: 'templates',
            files: 'files',
            settings: 'settings'
        }
    },

    // File Upload Limits
    fileUpload: {
        icon: {
            maxSize: 5 * 1024 * 1024, // 5MB
            allowedTypes: ['image/jpeg', 'image/jpg', 'image/png', 'image/bmp'],
            allowedExtensions: ['.jpg', '.jpeg', '.png', '.bmp']
        },
        mesh: {
            maxSize: 50 * 1024 * 1024, // 50MB
            allowedTypes: ['application/octet-stream', 'model/obj', 'model/fbx'],
            allowedExtensions: ['.obj', '.fbx']
        }
    },

    // Validation Rules
    validation: {
        itemName: {
            maxLength: 24,
            pattern: /^[a-zA-Z_]+$/,
            allowWhitespace: false
        },
        itemID: {
            pattern: /^[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/i
        },
        maxStackSize: {
            min: 1,
            max: 999999
        },
        weight: {
            min: 0,
            max: 999999
        },
        value: {
            min: 0,
            max: 999999999
        },
        durability: {
            min: 0,
            max: 100
        }
    },

    // Default Settings
    defaults: {
        categories: [
            'Weapon',
            'Armor', 
            'Consumable',
            'Material',
            'Quest',
            'Misc'
        ],
        rarities: [
            { name: 'Common', color: '#9CA3AF' },
            { name: 'Uncommon', color: '#10B981' },
            { name: 'Rare', color: '#3B82F6' },
            { name: 'Epic', color: '#8B5CF6' },
            { name: 'Legendary', color: '#F59E0B' }
        ],
        equipmentSlots: [
            'none',
            'Hand.Left',
            'Hand.Right', 
            'Back',
            'Belt',
            'Helmet',
            'Chest',
            'Shoulder.Left',
            'Shoulder.Right',
            'Leg.Left',
            'Leg.Right',
            'Boots',
            'Gloves',
            'Necklace'
        ],
        template: {
            itemName: '',
            displayName: '',
            thumbnailDescription: '',
            description: '',
            itemType: 'Misc',
            rarity: 'Common',
            maxStackSize: 1,
            weight: 0,
            value: 0,
            durability: 100,
            isStackable: true,
            isDroppable: true,
            isUsable: false,
            isEquippable: false,
            isTradeable: true,
            isQuestItem: false,
            iconFileId: null,
            meshFileId: null,
            meshPath: '',
            materialPath: '',
            equipSlot: 'None',
            customProperties: []
        }
    },

    // UI Configuration
    ui: {
        notifications: {
            duration: {
                success: 3000,
                info: 3000,
                warning: 4000,
                error: 5000
            },
            maxVisible: 5
        },
        pagination: {
            defaultPageSize: 50,
            maxPageSize: 200
        },
        preview: {
            autoUpdate: true,
            debounceTime: 300
        },
        validation: {
            realTime: true,
            debounceTime: 300
        }
    },

    // Export/Import Configuration
    export: {
        formats: {
            json: '.json',
            csv: '.csv',
            singleItem: '.mnteaitem',
            multipleItems: '.mnteaitems'
        },
        compression: {
            level: 6,
            type: 'DEFLATE'
        }
    },

    // External Libraries
    libraries: {
        jszip: {
            url: 'https://cdnjs.cloudflare.com/ajax/libs/jszip/3.10.1/jszip.min.js',
            version: '3.10.1'
        }
    },

    // Application Metadata
    app: {
        name: 'Mountea Inventory Template Editor',
        version: '1.0.0',
        author: 'Mountea Framework',
        description: 'Manage your inventory item templates online from any device',
        website: 'https://github.com/Mountea-Framework'
    },

    // Performance Settings
    performance: {
        maxTemplates: 10000,
        maxConcurrentUploads: 3,
        chunkSize: 1024 * 1024, // 1MB chunks for large file processing
        autoSaveInterval: 30000 // 30 seconds
    },

    // Error Messages
    errors: {
        database: {
            initFailed: 'Failed to initialize database',
            saveFailed: 'Failed to save data',
            loadFailed: 'Failed to load data',
            deleteFailed: 'Failed to delete data'
        },
        validation: {
            required: 'This field is required',
            invalidFormat: 'Invalid format',
            tooLong: 'Value is too long',
            tooShort: 'Value is too short',
            outOfRange: 'Value is out of range'
        },
        files: {
            tooLarge: 'File size exceeds maximum limit',
            invalidType: 'Invalid file type',
            uploadFailed: 'File upload failed',
            notFound: 'File not found'
        },
        general: {
            networkError: 'Network connection error',
            unknownError: 'An unknown error occurred',
            operationFailed: 'Operation failed'
        }
    },

    // Success Messages  
    success: {
        template: {
            created: 'Template created successfully',
            updated: 'Template updated successfully',
            deleted: 'Template deleted successfully',
            duplicated: 'Template duplicated successfully',
            imported: 'Template imported successfully',
            exported: 'Template exported successfully'
        },
        file: {
            uploaded: 'File uploaded successfully',
            deleted: 'File deleted successfully',
            replaced: 'File replaced successfully'
        },
        settings: {
            saved: 'Settings saved successfully',
            reset: 'Settings reset to defaults',
            imported: 'Settings imported successfully',
            exported: 'Settings exported successfully'
        }
    },

    // Keyboard Shortcuts
    shortcuts: {
        save: 'Ctrl+S',
        newTemplate: 'Ctrl+N',
        duplicate: 'Ctrl+D',
        export: 'Ctrl+E',
        help: 'F1',
        escape: 'Escape'
    },

    // Local Storage Keys
    storage: {
        lastSelectedTemplate: 'inventory_last_template',
        userPreferences: 'inventory_preferences',
        windowState: 'inventory_window_state'
    },

    // CSS Classes
    css: {
        error: 'error',
        success: 'success',
        warning: 'warning',
        loading: 'loading',
        hidden: 'hidden',
        active: 'active',
        selected: 'selected'
    },

    // Development/Debug Settings
    debug: {
        enabled: false,
        logLevel: 'info', // 'debug', 'info', 'warn', 'error'
        showPerformanceMetrics: false,
        mockData: false
    }
};

// Freeze the configuration to prevent accidental modifications
Object.freeze(AppConfig);