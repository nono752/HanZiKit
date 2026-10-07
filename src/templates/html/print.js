function printGlobalVocab() {
  const printZone = document.getElementById('print-zone');
  const today = new Date().toLocaleDateString();
  let html = `
    <div class="print-header-custom">
      <div class="print-header">${today}</div>
      <div class="print-header"> HanZiKit</div>
    </div>
    <h1 class="print-title"> ${DECK_DATA.title}</h1>`;

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
  const today = new Date().toLocaleDateString();
  let html = `
    <div class="print-header-custom">
      <div class="print-header">${today}</div>
      <div class="print-header"> HanZiKit</div>
    </div>
    <h1 class="print-title">${title}</h1><div class="print-grid-container">`;

  items.forEach(item => {
    const chars = Array.from(item.hanzi || item.traditional);
    // On découpe le pinyin en utilisant l'espace généré par ton outil C++
    const pinyins = (item.pinyin || '').split(' '); 
    const wordLen = chars.length;

    // Nouvelle structure : un conteneur parent Flexbox
    let charInfoHtml = `<div class="print-char-info">`;
    
    // On crée une sous-colonne pour chaque paire Pinyin/Hanzi
    chars.forEach((char, idx) => {
      const py = pinyins[idx] || ''; // Sécurité si un pinyin manque
      charInfoHtml += `
        <div class="char-pair">
          <div class="sub-pinyin">${py}</div>
          <div class="main-char">${char}</div>
        </div>
      `;
    });
    
    charInfoHtml += `</div>`;

    html += `
      <div class="print-grid-row">
        ${charInfoHtml}
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
  printZone.innerHTML = buildGridSheetHtml(`${DECK_DATA.title}`, allItems);
  window.print();
}

// Impression des grilles pour le Module actif[cite: 1]
function printModuleGrid() {
  if (currentModuleIndex < 0 || !DECK_DATA.modules[currentModuleIndex]) return;
  const mod = DECK_DATA.modules[currentModuleIndex];
  
  const printZone = document.getElementById('print-zone');
  printZone.innerHTML = buildGridSheetHtml(`${mod.title}`, mod.vocItems || []);
  window.print();
}