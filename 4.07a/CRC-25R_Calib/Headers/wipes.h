void WipesInitialize(void);
WELLWIPETYPE *WipesCurrentType(int type);
int ActiveWipeLocationCount(void);
WELLWIPELOCATION *ActiveWipeLocations(void);
void DeleteWipeLocation(WELLWIPELOCATION *delItem);
void UpdateWipeLocation(WELLWIPELOCATION *updateItem);
int FindWipeLocation(char *locationName, long long int WellWipeLocationID);
void InitializeAvailableWipeLocations(void);
void RemoveAllSelectedWipeLocations(void);
void AddAllAvailableWipeLocations(void);
int AvailableWipeLocationCount(void);
WELLWIPELOCATION *AvailableWipeLocations(void);

void SelectWipeLocationClearIndex(void);
int GetSelectWipeLocationIndex(void);
void IncSelectWipeLocationIndex(void);
void EndSelectWipeLocationIndex(void);
int SelectedWipeLocationCount(void);
WELLWIPELOCATION *SelectedWipeLocations(void);

void AddAvailableWipeLocation(int index);
void RemoveSelectedWipeLocation(int index);
bool TestWipeLocationNuclideMissingE1(int locationIndex, int nuclide[10]);
bool TestWipeLocationNuclideMissingEfficiency(int locationIndex, int nuclide[10]);

void RefreshWipeNuclides(void);
WELLWIPENUCLIDE *GetWipeNuclide(int NuclideID);

void DB_CreateWellWipe(WELLWIPE *wellWipe, bool bookEnd);
