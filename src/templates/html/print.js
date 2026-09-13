function printGlobalVocab() {
  const printZone = document.getElementById('print-zone');
  let html = `<h1 class="print-title"> ${DECK_DATA.title}</h1>`;
  html += `<table class="print-table">
            <thead>
              <tr>
                <th>Hanzi</th>
                <th>Pinyin</th>
                <th>Traduction</th>
              </tr>
            </thead>
            <tbody>`;

  // On boucle sur tous les modules pour extraire les mots[cite: 1]
  DECK_DATA.modules.forEach(mod => {
    // Ligne de titre du module (Design moderne)
    html += `
      <tr class="print-module-row">
        <td colspan="3">${mod.title}</td>
      </tr>`;
      
    if (mod.vocItems) {
      mod.vocItems.forEach(item => {
        html += `
          <tr>
            <td class="print-hanzi">${item.hanzi}</td>
            <td>${item.pinyin}</td>
            <td>${item.translation}</td>
          </tr>`;
      });
    }
  });

  html += `</tbody></table>`;
  
  // On injecte le tableau dans le DOM et on lance l'impression
  printZone.innerHTML = html;
  window.print();
}

function printModuleVocab() {
  if (currentModuleIndex < 0 || !DECK_DATA.modules[currentModuleIndex]) return;

  const mod = DECK_DATA.modules[currentModuleIndex];
  const printZone = document.getElementById('print-zone');

  let html = `<h1 class="print-title">${mod.title}</h1>`;
  html += `
    <table class="print-table">
      <thead>
        <tr>
          <th>Hanzi</th>
          <th>Pinyin</th>
          <th>Traduction</th>
        </tr>
      </thead>
      <tbody>`;

  if (mod.vocItems) {
    mod.vocItems.forEach(item => {
      html += `
        <tr>
          <td class="print-hanzi">${item.hanzi}</td>
          <td>${item.pinyin}</td>
          <td>${item.translation}</td>
        </tr>`;
    });
  }

  html += `</tbody></table>`;

  printZone.innerHTML = html;
  window.print();
}

// Générateur HTML pour une collection d'items
function buildGridSheetHtml(title, items, totalBoxes = 9, guideCount = 3) {
  let html = `<h1 class="print-title">${title}</h1><div class="print-grid-container">`;

  items.forEach(item => {
    const chars = item.hanzi.split('');
    chars.forEach((char, idx) => {
      html += `
        <div class="print-grid-row">
          <div class="print-char-info">
            <div class="main-char">${char}</div>
            <div class="sub-pinyin">${idx === 0 ? item.pinyin : ''}</div>
            <div class="sub-trans">${idx === 0 ? item.translation : ''}</div>
          </div>
          <div class="tzg-container">`;

      for (let i = 0; i < totalBoxes; i++) {
        // Affiche le modèle estompé sur les premières cases
        const hasGuide = i < guideCount;
        html += `
          <div class="tzg-cell">
            ${hasGuide ? `<div class="tzg-guide">${char}</div>` : ''}
          </div>`;
      }

      html += `</div></div>`;
    });
  });

  html += `</div>`;
  return html;
}

// Impression des grilles pour le Hub (Tous les modules)
function printGlobalGrid() {
  let allItems = [];
  DECK_DATA.modules.forEach(mod => {
    if (mod.vocItems) allItems = allItems.concat(mod.vocItems);
  });

  const printZone = document.getElementById('print-zone');
  printZone.innerHTML = buildGridSheetHtml(`Grilles d'écriture - ${DECK_DATA.title}`, allItems);
  window.print();
}

// Impression des grilles pour le Module actif
function printModuleGrid() {
  if (currentModuleIndex < 0 || !DECK_DATA.modules[currentModuleIndex]) return;
  const mod = DECK_DATA.modules[currentModuleIndex];
  
  const printZone = document.getElementById('print-zone');
  printZone.innerHTML = buildGridSheetHtml(`Grille : ${mod.title}`, mod.vocItems || []);
  window.print();
}