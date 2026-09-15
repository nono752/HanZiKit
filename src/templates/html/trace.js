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
  
  // Mise à jour des textes
  document.getElementById('trace-translation').textContent = item.translation;
  document.getElementById('trace-pinyin').textContent = item.pinyin;
  
  // Nettoie l'ancien tracé
  document.getElementById('character-target').innerHTML = '';
  
  // Initialisation de HanziWriter
  writer = HanziWriter.create('character-target', item.hanzi, {
    width: 200,
    height: 200,
    showOutline: true,
    strokeAnimationSpeed: 1,
    delayBetweenStrokes: 50
  });
  
  // Lance le mode interactif (Quiz)[cite: 1, 7]
  writer.quiz({
    onComplete: function() {
      // Passe au mot suivant automatiquement après 1 seconde en cas de réussite
      setTimeout(nextTrace, 1000);
    }
  });
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