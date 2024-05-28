const fs = require('fs');

let cachedPages = {};

module.exports.GetCachedPage = function(filename) {
    if (cachedPages[filename]) {
        return cachedPages[filename];
    }
    pageText = fs.readFileSync(filename, 'utf8');
    cachedPages[filename] = pageText;
    return cachedPages[filename];
}