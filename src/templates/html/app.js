function initHub() {
  if (!DECK_DATA || !DECK_DATA.title) return;

  document.getElementById('main-title').textContent = DECK_DATA.title;
  if (DECK_DATA.subtitle) {
    document.getElementById('main-subtitle').textContent = DECK_DATA.subtitle;
  }

  const grid = document.getElementById('hub-grid');
  grid.innerHTML = ''; 
  
  // On charge les stats globales une seule fois pour le Hub
  let stats = JSON.parse(localStorage.getItem('HanZiKit_stats') || '{}');
  
  DECK_DATA.modules.forEach((mod, index) => {
    const wordCount = mod.count || (mod.vocItems ? mod.vocItems.length : 0);
    
    // --- NOUVEAU : Calcul de la vraie progression du module ---
    let moduleSuccess = 0;
    let moduleTotal = 0;
    
    if (mod.vocItems) {
      mod.vocItems.forEach(item => {
        const charKey = item.hanzi || item.traditional;
        const wordStats = stats[charKey] || { success: 0, fail: 0 };
        moduleSuccess += wordStats.success;
        moduleTotal += (wordStats.success + wordStats.fail);
      });
    }
    
    let realProgress = 0;
    if (moduleTotal > 0) {
      realProgress = Math.round((moduleSuccess / moduleTotal) * 100);
    }
    // -----------------------------------------------------------
    
    grid.innerHTML += `
      <div class="module-card" onclick="openModule(${index}, '${mod.title}', ${wordCount})">
        <h3>${mod.title}</h3>
        <p style="color: var(--text-muted); font-size: 0.9rem; margin:0;">${wordCount} mots</p>
        <div class="progress-bar">
          <div class="progress-fill" style="width: ${realProgress}%;"></div>
        </div>
      </div>
    `;
  });
}

function openModule(index, title, count) {
  currentModuleIndex = index;
  document.getElementById('mod-title').textContent = title;
  document.getElementById('mod-count').textContent = count;
  
  const vocabContainer = document.getElementById('mod-vocab-list');
  if(vocabContainer) vocabContainer.innerHTML = ''; 
  
  const currentModule = DECK_DATA.modules[index];
  
  // Lecture des stats globales
  let stats = JSON.parse(localStorage.getItem('HanZiKit_stats') || '{}');
  
  let moduleSuccess = 0;
  let moduleTotal = 0;

  if (currentModule && currentModule.vocItems) {
    currentModule.vocItems.forEach(item => {
      const wordStats = stats[item.hanzi] || { success: 0, fail: 0 };
      const totalAttempts = wordStats.success + wordStats.fail;
      
      moduleSuccess += wordStats.success;
      moduleTotal += totalAttempts;

      let colorClass = '';
      let percentage = 0;
      
      // Logique des 50%
      if (totalAttempts > 0) {
        percentage = Math.round((wordStats.success / totalAttempts) * 100);
        if (wordStats.fail > wordStats.success) {
          colorClass = 'stat-red'; // Échoué plus de la moitié du temps
        } else if (wordStats.success > wordStats.fail) {
          colorClass = 'stat-green'; // Réussi plus de la moitié du temps
        }
      }

      vocabContainer.innerHTML += `
        <div class="vocab-item">
          <div class="vocab-hanzi ${colorClass}">${item.hanzi || item.traditional}</div>
          <div class="vocab-details">
            <div class="vocab-pinyin">${item.pinyin}</div>
            <div class="vocab-translation">${item.translation}</div>
          </div>
          
          <!-- NOUVEAU : Bloc Statistiques du mot -->
          <div class="vocab-stats">
             <div class="stat-text ${colorClass}">${totalAttempts > 0 ? percentage + '%' : '--'}</div>
             <div class="mastery-bar-bg">
                <div class="mastery-bar-fill ${colorClass}" style="width: ${percentage}%;"></div>
             </div>
          </div>
        </div>
      `;
    });
  }
  
  // Mise à jour du taux de réussite global tout en haut du dashboard
  const globalRateDisplay = document.getElementById('mod-success-rate');
  if (globalRateDisplay) {
    if (moduleTotal > 0) {
      globalRateDisplay.textContent = Math.round((moduleSuccess / moduleTotal) * 100) + '%';
    } else {
      globalRateDisplay.textContent = '--%';
    }
  }

  // Affichage de la vue
  document.querySelectorAll('.view').forEach(v => v.classList.remove('active'));
  document.getElementById('view-module').classList.add('active');
  window.scrollTo(0, 0); 
}

function showHub() {
// 1. On réinitialise la mémoire globale (Optionnel mais propre)
  currentModuleIndex = -1;

  // 2. On cache TOUTES les vues existantes d'un seul coup
  document.querySelectorAll('.view').forEach(v => v.classList.remove('active'));

  // 3. On réactive uniquement le Hub
  document.getElementById('view-hub').classList.add('active');
  window.scrollTo(0, 0);
}

function returnToModule() {
  // Sécurité : si aucun module n'est actif, on renvoie au Hub[cite: 1]
  if (currentModuleIndex < 0) {
    showHub();
    return;
  }
  
  const mod = DECK_DATA.modules[currentModuleIndex];
  const wordCount = mod.count || (mod.vocItems ? mod.vocItems.length : 0);
  
  // On rappelle openModule pour réafficher la vue ET recalculer les stats[cite: 1]
  openModule(currentModuleIndex, mod.title, wordCount);
}

window.onload = initHub;