# Wstępny opis

Scena składa się z 2 modelów kamieni, 1 modelu korala, 1 modelu ryby, 1 modelu podłoża

Regiony:
MenuItemsVariables - zawiera enum określający aspekt obrazu (panoramiczny, full-window, swobodna kamera), albo wyjście z programu

ControlVariables - zawiera zmienne ruchu, aktualną pozycję kamery, kąt nachylenia

GlobalVariables - zawiera zmienne shaderów, pozycję kamery, macierz perspektywy

Models - odpowiada za tworzenie zmiennych modeli oraz ich tekstur, zawiera region Fish

Fish - zawiera model i tekstury ryby

CoralGeneratorParameters - zmienne zawierające parametry generowania korali w losowych miejscach na mapie 

StoneGeneratorParameters - j.w., ale dla kamieni

BubbleGeneratorParameters - j.w., ale losuje również seedy dla trasy ruchu bąbelków 

Hierarchical Transformation - klasa obsługująca hierarchię macierzy informacji. Zawiera funkcję GetFinalTransform(), która wczytuje  drzewo transformacji (przemnażana jest macierz transformacji rodziców, aż do osiagnięcia korzenia). 

Constructors - należą do klasy Hierarchical Transformation
Setters and Getters - j.w.
Methods and functions - j.w.

CollisionFunctions - zbiór funkcji przeliczających kolizje ryby z obiektami (np.: skyboxem)

AdditionalFunctions - dodatkowe funkcje, umożliwiające działanie projektu, takie jak: ApplyWaveFunction(), UseCatmullRom(), CreateCameraMatrix() oraz zestaw setterów dla generatorów parametrów

DrawFunctions - funckej rysujące, korzystające z różnej kombinacji shaderów

Drawing complex shapes - funkcje rysujące zaawansowane kształty (ryba, korale, kamienie skybox) oraz funkcje inicjalizujące działanie programu

ProgramFlow - funkcje wyświetlania 
ReshapeFunctions - 
KeyFunctions - 
MenuFunctions - 
