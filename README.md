# Projet C++ : Ratp mapper

Pour pouvoir compiler le programme, il suffit d'utiliser le fichier makefile qui est inclus dans le dossier actuel comme suit : 

- Ouvrez un terminal 
- Assurez-vous que le logiciel Make est installé Sinon l'installer en utilisant : sudo apt-get install make
- Entrez "make" dans le terminal


Lorsque la compilation sera terminée, vous disposerez du fichier d'exécution sous le nom de "main".

Pour exécuter et utiliser le programme, vous pouvez procéder comme suit : 

- Ouvrez un terminal dans le meme dossier 
- Tappez : ./main chemin_vers_stations chemin_vers_connections

chemin_vers_stations : est le chemin relatif ou absolu vers le fichier csv des stations (ici data/s.csv), le chemin doit contenir aussi le nom du fichier
chemin_vers_connections : est le chemin relatif ou absolu vers le fichier csv des connections (ici data/c.csv) 
  