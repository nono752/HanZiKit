function startFlashcards() {
  if (currentModuleIndex < 0 || !DECK_DATA.modules[currentModuleIndex]) return;
  const mod = DECK_DATA.modules[currentModuleIndex];
  
  if (!mod.vocItems || mod.vocItems.length === 0) {
    alert("Aucun vocabulaire dans ce module.");
    return;
  }

  fcQueue = [...mod.vocItems];
  fcCurrentIndex = 0;
  
  document.getElementById('fc-title').textContent = "Flashcards : " + mod.title;
  
  document.querySelectorAll('.view').forEach(v => v.classList.remove('active'));
  document.getElementById('view-flashcards').classList.add('active');
  
  loadCurrentCard();
}

function loadCurrentCard() {
  if (fcCurrentIndex >= fcQueue.length) {
    alert("Module terminé !");
    returnToModule(); // Retour automatique au module[cite: 1]
    return;
  }

  const item = fcQueue[fcCurrentIndex];
  document.getElementById('fc-counter').textContent = `${fcCurrentIndex + 1} / ${fcQueue.length}`;
  
  document.getElementById('fc-hanzi').textContent = item.hanzi || item.traditional || "X";
  document.getElementById('fc-pinyin').textContent = item.pinyin || "--";
  document.getElementById('fc-translation').textContent = item.translation || "--";

  // Réinitialiser les 3 champs (remettre les masques)
  document.querySelectorAll('.fc-reveal-field').forEach(field => {
    field.classList.remove('revealed');
  });
}

// Révéler un champ spécifique au clic
function revealField(element) {
  element.classList.add('revealed');
}

// Passer la carte sans enregistrer de score
function skipCard() {
  // 1. On remet les masques immédiatement
  document.querySelectorAll('.fc-reveal-field').forEach(field => {
    field.classList.remove('revealed');
  });

  // 2. On attend la fin de la transition CSS (200ms) pour changer les données
  setTimeout(() => {
    fcCurrentIndex++;
    loadCurrentCard();
  }, 200);
}

// Lire l'audio pour la carte actuelle[cite: 2]
function speakFlashcardHanzi() {
  const item = fcQueue[fcCurrentIndex];
  const utterance = new SpeechSynthesisUtterance(item.hanzi || item.traditional);
  utterance.lang = 'zh-CN';
  utterance.rate = 0.85;
  window.speechSynthesis.speak(utterance);
}

// Enregistrer le score et passer à la suivante[cite: 1]
function nextCard(isSuccess) {
  const item = fcQueue[fcCurrentIndex];
  const charKey = item.hanzi || item.traditional;
  
  // Sauvegarde sécurisée dans le localStorage[cite: 1]
  let stats = {};
  try {
    stats = JSON.parse(localStorage.getItem('hanziforge_stats') || '{}');
  } catch (e) {}
  
  if (!stats[charKey]) stats[charKey] = { success: 0, fail: 0 };
  if (isSuccess) stats[charKey].success++;
  else stats[charKey].fail++;
  
  try {
    localStorage.setItem('hanziforge_stats', JSON.stringify(stats));
  } catch (e) {}

  // 1. On remet les masques immédiatement
  document.querySelectorAll('.fc-reveal-field').forEach(field => {
    field.classList.remove('revealed');
  });

  // 2. On attend 200ms pour que tout soit caché avant d'afficher la carte suivante
  setTimeout(() => {
    fcCurrentIndex++;
    loadCurrentCard();
  }, 200);
}