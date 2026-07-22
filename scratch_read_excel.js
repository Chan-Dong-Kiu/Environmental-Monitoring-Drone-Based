const XLSX = require('xlsx');
const fs = require('fs');

function readExcel(filename) {
    try {
        const workbook = XLSX.readFile(filename);
        const data = {};
        for (const sheetName of workbook.SheetNames) {
            const worksheet = workbook.Sheets[sheetName];
            data[sheetName] = XLSX.utils.sheet_to_json(worksheet, { header: 1 });
        }
        return data;
    } catch (e) {
        return { error: e.message };
    }
}

const templateData = readExcel('AI_AuditLog_Template.xlsx');
const existingData = readExcel('AI_AuditLog_SE2036_Group7_SE201129_NamLHSE201129.xlsx');

fs.writeFileSync('excel_data_utf8.json', JSON.stringify({
    template: templateData,
    existing: existingData
}, null, 2), 'utf8');
