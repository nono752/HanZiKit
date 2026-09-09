function initHub()
{
  if (!DECK_DATA || !DECK_DATA.title) return;

  document.getElementById('main-title').textContent = DECK_DATA.title;
  if (DECK_DATA.subtitle) {
    document.getElementById('main-subtitle').textContent = DECK_DATA.subtitle;
  } else {
    document.getElementById('main-subtitle').textContent = "Prêt pour la révision";
  }

  const grid = document.getElementById('hub-grid');
  grid.innerHTML = ''; 
  
  DECK_DATA.modules.forEach(mod => {
    // Le C++ écrira la taille du tableau dans 'count', on l'utilise directement
    const wordCount = mod.count || (mod.vocItems ? mod.vocItems.length : 0);
    const fakeProgress = Math.floor(Math.random() * 100); 
    
    grid.innerHTML += `
      <div class="module-card" onclick="openModule(${mod.id}, '${mod.title}', ${wordCount})">
        <h3>${mod.title}</h3>
        <p style="color: var(--text-muted); font-size: 0.9rem; margin:0;">${wordCount} mots</p>
        <div class="progress-bar">
          <div class="progress-fill" style="width: ${fakeProgress}%;"></div>
        </div>
      </div>
    `;
  });
}

function openModule(index, title, count) {
  // 1. Mise à jour de l'en-tête
  document.getElementById('mod-title').textContent = title;
  document.getElementById('mod-count').textContent = count;
  
  // 2. Génération de la liste de vocabulaire
  const vocabContainer = document.getElementById('mod-vocab-list');
  vocabContainer.innerHTML = ''; // On vide la liste précédente
  
  // On récupère le module cliqué dans le JSON global[cite: 1]
  const currentModule = DECK_DATA.modules[index];
  
  // Si le module contient des mots, on les affiche
  if (currentModule && currentModule.vocItems) {
    currentModule.vocItems.forEach(item => {
      // On crée une ligne HTML pour chaque mot avec les champs de ton AST[cite: 1]
      vocabContainer.innerHTML += `
        <div class="vocab-item">
          <div class="vocab-hanzi">${item.hanzi}</div>
          <div class="vocab-details">
            <div class="vocab-pinyin">${item.pinyin}</div>
            <div class="vocab-translation">${item.translation}</div>
          </div>
        </div>
      `;
    });
  }
  
  // 3. Bascule de l'affichage
  document.getElementById('view-hub').classList.remove('active');
  document.getElementById('view-module').classList.add('active');
  window.scrollTo(0, 0); 
}

function showHub() {
  document.getElementById('view-module').classList.remove('active');
  document.getElementById('view-hub').classList.add('active');
}

window.onload = initHub;