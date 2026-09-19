function startTrace() {
  const mod = DECK_DATA.modules[currentModuleIndex];
  if (!mod.vocItems || mod.vocItems.length === 0) {
    alert("Aucun mot à tracer dans ce module.");
    return;
  }
  
  currentTraceIndex = 0; // On commence par le premier mot
  
  // Bascule d'affichage
  document.getElementById('view-module').classList.remove('active');
  document.getElementById('view-trace').classList.add('active');
  
  loadTraceCard();
}

function loadTraceCard() {
  const item = DECK_DATA.modules[currentModuleIndex].vocItems[currentTraceIndex];
  const word = item.hanzi || item.traditional;
  
  // 1. Découpe le mot en un tableau de caractères individuels (ex: "电话" -> ["电", "话"])
  const chars = Array.from(word); 
  
  // Mise à jour des textes
  document.getElementById('trace-translation').textContent = item.translation || '';
  document.getElementById('trace-pinyin').textContent = item.pinyin || '';
  
  // 2. Nettoyage et adaptation du conteneur parent
  const targetContainer = document.getElementById('character-target');
  targetContainer.innerHTML = '';
  
  // On modifie le style du conteneur pour qu'il affiche les boîtes en ligne
  targetContainer.style.width = 'auto';
  targetContainer.style.border = 'none'; // On enlève la bordure globale
  targetContainer.style.display = 'flex';
  targetContainer.style.justifyContent = 'center';
  targetContainer.style.gap = '10px';
  targetContainer.style.flexWrap = 'wrap'; // Permet de passer à la ligne si le mot est très long
  
  let writers = []; // On va stocker tous les moteurs de tracé du mot
  
  // 3. Création d'une zone et d'un tracé pour CHAQUE caractère
  chars.forEach((char, index) => {
    // Création de la sous-boîte
    const charDiv = document.createElement('div');
    charDiv.id = 'trace-char-' + index;
    // On met la bordure en pointillés sur chaque petite boîte
    charDiv.style.width = '120px'; // Un peu plus petit pour que plusieurs rentrent à l'écran
    charDiv.style.height = '120px';
    charDiv.style.border = '2px dashed var(--border)';
    charDiv.style.borderRadius = '8px';
    
    targetContainer.appendChild(charDiv);
    
    // Initialisation du HanziWriter pour CE caractère précis[cite: 2]
    const w = HanziWriter.create(charDiv.id, char, {
      width: 120,
      height: 120,
      showOutline: true,
      strokeAnimationSpeed: 1,
      delayBetweenStrokes: 50
    });
    writers.push(w);
  });
  
  // 4. Fonction pour enchaîner le mode Quiz caractère par caractère
  function startQuizForChar(index) {
    // Si on a dépassé le dernier caractère, le mot entier est réussi !
    if (index >= writers.length) {
      setTimeout(nextTrace, 1000); // Passe au mot suivant de la liste[cite: 2]
      return;
    }
    
    // Sinon, on lance le quiz sur le caractère actuel[cite: 2]
    writers[index].quiz({
      onComplete: function() {
        // Dès qu'il est fini, on lance automatiquement le quiz du caractère suivant
        startQuizForChar(index + 1);
      }
    });
  }
  
  // 5. On déclenche le quiz sur le tout premier caractère du mot
  if (writers.length > 0) {
    startQuizForChar(0);
  }
}

function nextTrace() {
  const mod = DECK_DATA.modules[currentModuleIndex];
  currentTraceIndex = (currentTraceIndex + 1) % mod.vocItems.length;
  loadTraceCard();
}

// Audio Natif sans internet[cite: 1, 9]
function speakHanzi() {
  const item = DECK_DATA.modules[currentModuleIndex].vocItems[currentTraceIndex];
  const utterance = new SpeechSynthesisUtterance(item.hanzi);
  utterance.lang = 'zh-CN';
  utterance.rate = 0.85;
  window.speechSynthesis.speak(utterance);
}