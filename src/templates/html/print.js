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
function buildGridSheetHtml(title, items, totalBoxes = 12, wordGuideRepetitions = 2) {
  let html = `<h1 class="print-title">${title}</h1><div class="print-grid-container">`;

  items.forEach(item => {
    // Array.from gère mieux les caractères complexes UTF-8 que split('')
    const chars = Array.from(item.hanzi || item.traditional);
    const wordLen = chars.length;

    html += `
      <div class="print-grid-row">
        <div class="print-char-info">
          <!-- Le pinyin est désormais placé AU-DESSUS du mot -->
          <div class="sub-pinyin">${item.pinyin || ''}</div>
          <div class="main-char">${item.hanzi || item.traditional}</div>
        </div>
        <div class="tzg-container">`;

    for (let i = 0; i < totalBoxes; i++) {
      // Détermine quel caractère du mot va dans cette case (modulo)
      const char = chars[i % wordLen];
      
      // Détermine si on est encore dans les répétitions "guidées" du mot
      const isGuide = Math.floor(i / wordLen) < wordGuideRepetitions;

      html += `
        <div class="tzg-cell">
          ${isGuide ? `<div class="tzg-guide">${char}</div>` : ''}
        </div>`;
    }

    html += `</div></div>`;
  });

  html += `</div>`;
  return html;
}

// Impression des grilles pour le Hub (Tous les modules)[cite: 1]
function printGlobalGrid() {
  let allItems = [];
  DECK_DATA.modules.forEach(mod => {
    if (mod.vocItems) allItems = allItems.concat(mod.vocItems);
  });

  const printZone = document.getElementById('print-zone');
  printZone.innerHTML = buildGridSheetHtml(`Grilles d'écriture - ${DECK_DATA.title}`, allItems);
  window.print();
}

// Impression des grilles pour le Module actif[cite: 1]
function printModuleGrid() {
  if (currentModuleIndex < 0 || !DECK_DATA.modules[currentModuleIndex]) return;
  const mod = DECK_DATA.modules[currentModuleIndex];
  
  const printZone = document.getElementById('print-zone');
  printZone.innerHTML = buildGridSheetHtml(`Grille : ${mod.title}`, mod.vocItems || []);
  window.print();
}