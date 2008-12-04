#ifndef HEURISTICS_H_
#define HEURISTICS_H_

#include "GlobalParams.h"
#include "WebFile.h"

/**
 * @class Heuristics
 * @brief Classe que que encapsula les heurístiques de l'aplicació.
 * 
 * Classe estàtica que conté tots els mètodes necessaris per a aplicar les heurístiques especificades
 * per a diferenciar els fitxers entre ells. Aquestes heurístiques són bàsicament:
 * -La diferenciació per mitjan de la comparació de les etiquetes HTML, combinada amb la comparació
 * de la longitud dels bloc de text inserits entre elles.
 * -La comparació de la mida dels fitxers en bytes.
 * -La comparació de les extensions dels fitxers.
 * -La diferència en la profunditat en l'arbre de directoris (aquesta és la única heurística que no
 * es troba implementada en esta classe, sinó en la classe <code>WebSite<code>, al mètode <code>GetMatchedFiles</code>. 
 * 
 * @author Miquel Esplà i Gomis
 */
class Heuristics
{
public:
	/**
	 * Mètode que indica si dos fitxers web poden ser el mateix basant-se l'extensió.
	 * @param wf1 Paràmetre de tipus <code>WebFile</code> que conté la informació sobre un dels fitxers web amb el qual hem de fer la comparació.
	 * @param wf2 Paràmetre de tipus <code>WebFile</code> que conté la informació sobre l'altre fitxer web amb el qual hem de fer la comparació.
	 * @return Retorna <code>true</code> si l'extensió d'ambdós fitxers és la mateixa, o <code>false</code> en cas contrari. 
	 */
	static bool HaveTheSameExtension(WebFile wf1, WebFile wf2);
	
	/**
	 * Mètode que indica si dos fitxers web poden ser el mateix basant-se en la seua mida en bytes. Per a activar aquest mètode cal donar un percentatge
	 * llindar per a la diferència de mida als paràmetres globals. Si no s'activa aquest llindar, el mètode retornarà sempre <code>true</code>.
	 * @param wf1 Paràmetre de tipus <code>WebFile</code> que conté la informació sobre un dels fitxers web amb el qual hem de fer la comparació.
	 * @param wf2 Paràmetre de tipus <code>WebFile</code> que conté la informació sobre l'altre fitxer web amb el qual hem de fer la comparació.
	 * @param result Paràmetre que serveix per a obtenir el percentatge de diferència de mida entre els dos fitxers. Si no es defineix el paràmetre, aquest adopta el valor NULL per defecte.
	 * @return Retorna <code>true</code> si la diferència de mida dels fitxers és acceptable segons els paràmetres establerts, o <code>false</code> en cas contrari. 
	 */
	static bool HaveAcceptableSizeDifference(WebFile wf1, WebFile wf2, float* result);
	
	/**
	 * Mètode que calcula la distància d'edició entre dues cadenes d'etiquetes HTML/Text tal com s'estableixen a la classe WebFile.
	 * Aquest mètode es basa en dos paràmetres globals. D'una banda, utilitza el llindar de percentatge de diferència entre longituds
	 * de text. Quan es calcula la distància d'edició entre dos blocs de text, si superen aquest percentatge, es consideren diferents
	 * i, en cas contrari, iguals. En cas que el paràmetre es trobe desactivat, es consideraran tots els blocs de text iguals.
	 * El segon paràmetre global és el del llindar de màxim percentatge de distància d'edició entre dos vectors d'etiquetes HTML/Text.
	 * Si aquest llindar està activat, la distància d'edició es limita, de tal forma que, en comptes de calcular-se tota la taula,
	 * es calcula només una diagonal determinada, basant-se en què tots els valors fora d'aquesta diagonal condueixen a resultats que
	 * excedeixen el llindar establert. La amplada d'aquesta diagonal serà directament proporcional al propi llindar.
	 * @param wf1 Paràmetre de tipus <code>WebFile</code> que conté la informació sobre un dels fitxers web amb el qual hem de fer la comparació.
	 * @param wf2 Paràmetre de tipus <code>WebFile</code> que conté la informació sobre l'altre fitxer web amb el qual hem de fer la comparació.
	 * @param result Paràmetre que serveix per a obtenir la distància d'edició entre els vectors d'etiquetes HTML/Text dels dos fitxers. Si no es defineix el paràmetre, aquest adopta el valor NULL per defecte.
	 * @return Retorna la distància d'edició calculada com a enter major o igual a zero. En cas que la distància excedisca el màxim establert, el mètode retornarà -1.
	 */
	static bool HaveAcceptableEditDistance(WebFile wf1, WebFile wf2, float* result);
	
	/**
	 * Mètode auxiliar que calcula el menor de tres enters. Si un dels paràmetres val -1 no serà
	 * tingut en compte.
	 * @param v1 Primer valor de la comparació.
	 * @param v2 Segon valor de la comparació.
	 * @param v3 Tercer valor de la comparació.
	 * @param infinite Un enter que és major que qualsevol dels possibles valors passats pels altres
	 * tres paràmetres. Per tant, es pot utilitzar aquest com a valor d'infinit, ja que cap dels enters
	 * a evaluar el superaran mai.
	 * @return Valor escollit. Si tots tres paràmetres valen -1, es retorna -1, indicant que cap valor
	 * és acceptable per a la distància d'edició.
	 */
	static int GetMinorValue(int vl1, int vl2, int vl3, int infinite);
};

#endif /*HEURISTICS_H_*/
