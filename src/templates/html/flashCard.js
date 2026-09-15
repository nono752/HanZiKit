// 1. Lancer le mode flashcard pour un module spécifique
function startFlashcards() {
  const mod = DECK_DATA.modules[currentModuleIndex];
  if (!mod.vocItems || mod.vocItems.length === 0) {
    alert("Aucun vocabulaire dans ce module.");
    return;
  }

  // Initialisation de la file d'attente
  fcQueue = [...mod.vocItems];
  // Optionnel : fcQueue.sort(() => Math.random() - 0.5); // Mélanger
  fcCurrentIndex = 0;
  
  document.getElementById('fc-title').textContent = "Flashcards : " + mod.title;
  
  // Bascule d'affichage
  document.querySelectorAll('.view').forEach(v => v.classList.remove('active'));
  document.getElementById('view-flashcards').classList.add('active');
  
  loadCurrentCard();
}

// 2. Charger les données sur la carte
function loadCurrentCard() {
  if (fcCurrentIndex >= fcQueue.length) {
    alert("Module terminé !");
    returnToModule();
    return;
  }

  const item = fcQueue[fcCurrentIndex];
  document.getElementById('fc-counter').textContent = `${fcCurrentIndex + 1} / ${fcQueue.length}`;
  
  document.getElementById('fc-hanzi').textContent = item.hanzi || item.traditional || "X";
  document.getElementById('fc-pinyin').textContent = item.pinyin || "--";
  document.getElementById('fc-translation').textContent = item.translation || "--";

  // Réinitialiser l'état visuel
  isCardFlipped = false;
  document.getElementById('fc-inner').classList.remove('is-flipped');
  document.getElementById('fc-controls').style.display = 'none';
}

// 3. Retourner la carte
function flipCard() {
  if (isCardFlipped) return; // Empêche de recliquer si déjà retournée
  
  isCardFlipped = true;
  document.getElementById('fc-inner').classList.add('is-flipped');
  
  // Afficher les boutons d'évaluation après l'animation
  setTimeout(() => {
    document.getElementById('fc-controls').style.display = 'flex';
  }, 300);
}

// 4. Évaluer et passer à la suivante
function nextCard(isSuccess) {
  const item = fcQueue[fcCurrentIndex];
  const charKey = item.hanzi || item.traditional;
  
  // Sauvegarde dans le localStorage
  let stats = JSON.parse(localStorage.getItem('HanZiKit_stats') || '{}');
  if (!stats[charKey]) stats[charKey] = { success: 0, fail: 0 };
  
  if (isSuccess) {
    stats[charKey].success++;
  } else {
    stats[charKey].fail++;
  }
  localStorage.setItem('HanZiKit_stats', JSON.stringify(stats));

  // Mot suivant
  fcCurrentIndex++;
  loadCurrentCard();
}

function saveWordStat(hanzi, isSuccess) {
  // On récupère les stats existantes ou on crée un objet vide[cite: 1]
  let stats = JSON.parse(localStorage.getItem('HanZiKit_stats') || '{}');
  
  // Initialisation du mot s'il n'existe pas encore
  if (!stats[hanzi]) {
    stats[hanzi] = { success: 0, fail: 0 };
  }
  
  // Incrémentation
  if (isSuccess) {
    stats[hanzi].success++;
  } else {
    stats[hanzi].fail++;
  }
  
  // Sauvegarde dans le cache du navigateur[cite: 1]
  localStorage.setItem('HanZiKit_stats', JSON.stringify(stats));
}