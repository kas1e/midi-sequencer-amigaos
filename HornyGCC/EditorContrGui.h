void InitController(void);
void ZeichneControllerSpalten(void);
void ZeichneContr(WORD n, struct EVENT *lcev, struct EVENT *fcev, BOOL mitte, BOOL del);
void ZeichneContrVorschau(WORD n, struct EVENT *ev, BOOL mitte);
void ZeichneControllerSpur(WORD n);
void ZeichneControllerFeld(BOOL anders);
void ContrSpurenEinpassen(void);
void ZeichneEdContrInfobox(void);
WORD PunktContrSpur(WORD my);
BYTE PunktContrWert(WORD my, WORD cs, BOOL onoff);
void ContrSpurAktivieren(WORD n);
