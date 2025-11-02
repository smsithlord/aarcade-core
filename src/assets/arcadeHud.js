/**
 * Arcade HUD - JavaScript manager for Arcade Core functionality
 *
 * Provides a simple, unified API for working with the native C++ bridge (aapi).
 * Handles background tasks like polling for image completions, managing intervals, etc.
 */

const arcadeHud = (function() {
    'use strict';

    // Private state
    let imageCompletionInterval = null;
    let isInitialized = false;

    /**
     * Initialize the arcade HUD
     * Sets up polling intervals and other background tasks
     */
    function initialize() {
        if (isInitialized) {
            console.warn('[arcadeHud] Already initialized');
            return;
        }

        console.log('[arcadeHud] Initializing...');

        // Check if aapi is available
        if (typeof aapi === 'undefined') {
            console.error('[arcadeHud] aapi not available. Make sure this is running in Arcade Core.');
            return false;
        }

        // Start polling for image completions if the method exists
        if (typeof aapi.processImageCompletions === 'function') {
            startImageCompletionPolling();
        } else {
            console.warn('[arcadeHud] aapi.processImageCompletions not available');
        }

        isInitialized = true;
        console.log('[arcadeHud] Initialized successfully');
        return true;
    }

    /**
     * Start polling for image download completions
     * @private
     */
    function startImageCompletionPolling() {
        if (imageCompletionInterval) {
            return; // Already running
        }

        console.log('[arcadeHud] Starting image completion polling');
        imageCompletionInterval = setInterval(function() {
            try {
                aapi.processImageCompletions();
            } catch (e) {
                console.error('[arcadeHud] Error processing image completions:', e);
            }
        }, 50); // Poll every 50ms
    }

    /**
     * Stop polling for image completions
     * @private
     */
    function stopImageCompletionPolling() {
        if (imageCompletionInterval) {
            clearInterval(imageCompletionInterval);
            imageCompletionInterval = null;
            console.log('[arcadeHud] Stopped image completion polling');
        }
    }

    /**
     * Load and cache an image from a URL
     *
     * The image is rendered in a 512x512 view with aspect ratio preservation (object-fit: contain).
     * The actual rendered image rect is calculated and the bitmap is cropped to only save the
     * actual image pixels, eliminating transparent borders.
     *
     * @param {string} url - The image URL to download and cache
     * @returns {Promise<Object>} Promise that resolves with image details:
     *   - filePath: Local file:// URL to the cached PNG
     *   - downloaderName: Name of the downloader used (e.g., "ImageLoader")
     *   - rectX: X coordinate of the image within the 512x512 canvas (before crop)
     *   - rectY: Y coordinate of the image within the 512x512 canvas (before crop)
     *   - rectWidth: Width of the actual image in pixels
     *   - rectHeight: Height of the actual image in pixels
     *
     * Note: The saved PNG file is cropped to rectWidth x rectHeight, maintaining the
     * original image's aspect ratio.
     */
    function loadImage(url) {
        if (!isInitialized) {
            console.warn('[arcadeHud] Not initialized, auto-initializing...');
            initialize();
        }

        if (typeof aapi === 'undefined' || typeof aapi.getCacheImage !== 'function') {
            return Promise.reject(new Error('Image caching not available'));
        }

        // getCacheImage returns a promise that resolves to {filePath, ...}
        // The C++ ImageLoader also provides rect info, but it's not currently
        // exposed through the Promise. The rect data is used internally for cropping.
        return aapi.getCacheImage(url);
    }

    /**
     * Get supported entry types from the database
     * @returns {Array<string>} Array of supported entry type names
     */
    function getSupportedEntryTypes() {
        if (typeof aapi === 'undefined' || typeof aapi.getSupportedEntryTypes !== 'function') {
            console.error('[arcadeHud] getSupportedEntryTypes not available');
            return [];
        }

        return aapi.getSupportedEntryTypes();
    }

    /**
     * Shutdown the arcade HUD
     * Stops all background tasks and cleans up resources
     */
    function shutdown() {
        console.log('[arcadeHud] Shutting down...');
        stopImageCompletionPolling();
        isInitialized = false;
    }

    /**
     * Check if the arcade HUD is initialized
     * @returns {boolean} True if initialized
     */
    function isReady() {
        return isInitialized;
    }

    /**
     * Check if a specific feature is available
     * @param {string} feature - Feature name (e.g., 'imageCache', 'database')
     * @returns {boolean} True if feature is available
     */
    function hasFeature(feature) {
        if (typeof aapi === 'undefined') {
            return false;
        }

        switch (feature) {
            case 'imageCache':
                return typeof aapi.getCacheImage === 'function' &&
                       typeof aapi.processImageCompletions === 'function';
            case 'database':
                return typeof aapi.getFirstEntries === 'function';
            case 'search':
                return typeof aapi.getFirstSearchResults === 'function';
            default:
                return false;
        }
    }

    // Public API
    return {
        initialize: initialize,
        shutdown: shutdown,
        isReady: isReady,
        hasFeature: hasFeature,

        // Image loading
        loadImage: loadImage,

        // Database
        getSupportedEntryTypes: getSupportedEntryTypes,

        // Direct access to aapi (for advanced usage)
        get aapi() {
            return typeof aapi !== 'undefined' ? aapi : null;
        }
    };
})();

// Auto-initialize when the DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', function() {
        arcadeHud.initialize();
    });
} else {
    // DOM already loaded
    arcadeHud.initialize();
}

// Cleanup on page unload
window.addEventListener('beforeunload', function() {
    arcadeHud.shutdown();
});
