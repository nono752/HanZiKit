function startTrace() {
  const mod = DECK_DATA.modules[currentModuleIndex];
  if (!mod.vocItems || mod.vocItems.length === 0) {
    alert("Aucun mot à tracer dans ce module.");
    return;
  }
  currentTraceIndex = 0;
  
  document.querySelectorAll('.view').forEach(v => v.classList.remove('active'));
  document.getElementById('view-trace').classList.add('active');
  
  loadTraceCard();
}

function loadTraceCard() {
  const item = DECK_DATA.modules[currentModuleIndex].vocItems[currentTraceIndex];
  const word = Array.from(item.hanzi || item.traditional || "");
  if (word.length === 0) return;
  
  document.getElementById('trace-translation').textContent = item.translation || '';
  document.getElementById('trace-pinyin').textContent = item.pinyin || '';
  
  const targetContainer = document.getElementById('trace-targets-container');
  const decompContainer = document.getElementById('stroke-decomposition');
  
  // Nettoyage complet
  targetContainer.innerHTML = '';
  decompContainer.innerHTML = '';
  currentWriters = [];
  currentCharQuizIndex = 0;
  
  word.forEach((char, index) => {
    // --- 1. Instanciation de la case principale interactive ---
    const boxId = 'trace-main-' + index;
    const box = document.createElement('div');
    box.id = boxId;
    box.className = 'trace-target-box';
    targetContainer.appendChild(box);
    
    const writer = HanziWriter.create(boxId, char, {
      width: 150, height: 150,
      showOutline: true, strokeAnimationSpeed: 1, delayBetweenStrokes: 50
    });
    currentWriters.push({ instance: writer, element: box });
    
    // --- 2. RESTAURATION DE LA LIGNE HORIZONTALE (.char-decomp-row) ---
    // On crée un conteneur flex horizontal pour CE sinogramme
    const decompRow = document.createElement('div');
    decompRow.className = 'char-decomp-row';
    decompContainer.appendChild(decompRow);
    
    // --- 3. Génération des petits SVG ---
    HanziWriter.loadCharacterData(char).then(function(charData) {
      if (!charData || !charData.strokes) return;
      const numStrokes = charData.strokes.length;
      
      for (let i = 0; i < numStrokes; i++) {
        const stepBox = document.createElement('div');
        stepBox.className = 'stroke-box';
        
        let svg = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1024 1024" width="34" height="34">
                     <g transform="scale(1, -1) translate(0, -900)">`;
                     
        for (let j = 0; j < numStrokes; j++) svg += `<path d="${charData.strokes[j]}" fill="#ecf0f1" />`;
        for (let j = 0; j <= i; j++) svg += `<path d="${charData.strokes[j]}" fill="#2c3e50" />`;
        
        svg += `</g></svg>`;
        stepBox.innerHTML = svg;
        
        // CORRECTION : On ajoute la case dans sa ligne horizontale (decompRow), pas dans le conteneur global !
        decompRow.appendChild(stepBox);
      }
    }).catch(function(err) {
      console.error("Données de tracé introuvables pour : " + char, err);
    });
  });
  
  // Lancement du quiz interactif sur le premier caractère
  startNextCharQuiz();
}

function startNextCharQuiz() {
  if (currentCharQuizIndex < currentWriters.length) {
    // Met en valeur la case à tracer
    currentWriters.forEach(w => w.element.classList.remove('active-quiz'));
    currentWriters[currentCharQuizIndex].element.classList.add('active-quiz');
    
    // Lance le mode interactif (Quiz)[cite: 1, 7]
    currentWriters[currentCharQuizIndex].instance.quiz({
      onComplete: function() {
        currentWriters[currentCharQuizIndex].element.classList.remove('active-quiz');
        currentCharQuizIndex++;
        
        // Passe à la lettre suivante ou au mot suivant[cite: 7]
        if (currentCharQuizIndex < currentWriters.length) {
          setTimeout(startNextCharQuiz, 300);
        } else {
          setTimeout(nextTrace, 1000);
        }
      }
    });
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