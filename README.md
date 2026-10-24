# Route 66 : The Hollow Mile

Projet de Programmation Graphique 3D réalisé en C++ avec OpenGL.

Il s'agit d'une scène 3D interactive représentant un tronçon isolé de la Route 66 de nuit (avec possibilité de basculer en mode jour). La scène inclut un motel rétro, une station-service, des voitures en circulation avec leurs phares, un feu de camp animé, un terrain avec relief et une ambiance mystérieuse inspirée des légendes urbaines américaines.

## Fonctionnalités

- **Moteur de rendu OpenGL** : Shaders personnalisés (Vertex / Fragment) gérant l'éclairage de Phong/Blinn-Phong.
- **Système d'éclairage complet** :
  - Lumière directionnelle (Lune / Soleil selon le cycle jour/nuit)
  - Lampadaires et enseignes lumineuses (Point lights)
  - Phares des voitures et lampe torche du joueur (Spotlights)
  - Feu de camp scintillant
- **Objets & Modèles 3D (.obj)** : Voitures (AMG GT, GTR, M3, Porsche, Tesla...), motel, station essence, arbres, tentes, barils, etc.
- **Terrain** : Génération de relief/heightmap avec texture personnalisée.
- **Textures** : Chargement via `stb_image` avec gestion du mapping UV.
- **Caméras** :
  - Mode caméra cinématique automatique qui survole la scène
  - Mode caméra libre (FPS) avec contrôles clavier/souris

## Contrôles

| Touche | Action |
|---|---|
| **Z / Q / S / D** (ou W/A/S/D) | Déplacement de la caméra libre |
| **Souris** | Orientation du regard |
| **C** | Activer / Désactiver la caméra cinématique |
| **F** | Activer / Éteindre la lampe torche |
| **L** | Basculer entre le mode Nuit et Jour |
| **W** | Activer / Désactiver le mode fil de fer (Wireframe) |
| **F11** | Mode plein écran |
| **Échap** | Quitter l'application |

## Dépendances & Compilation

- **IDE** : Visual Studio 2022 (Solution `.slnx` / `.vcxproj` incluse)
- **Bibliothèques utilisées** :
  - GLEW (OpenGL Extension Wrangler)
  - GLFW (gestion des fenêtres et inputs)
  - GLM (mathématiques 3D, matrices, vecteurs)
  - stb_image (chargement des textures)

Ouvrir la solution dans Visual Studio, s'assurer que la configuration est en `x64` (Debug ou Release) et lancer l'exécution (`F5`).
