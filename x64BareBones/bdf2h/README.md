# Description
Given a BFD font file, it outputs a C .h file with bitmap font definition.

# Requires fontforge intallation

If you want to start from a ttf font, you can obtain the .bdf file using fontforge. In ubuntu or mac, fontforge can do the trick.

For ubuntu:

``sudo apt install fontforge``

# Procedure to obtain .h bitmap font definition out of a .ttf

-- Abrir fuente con fontforge.

-- Si ``elements > bitmap strikes available`` se encuentra greseado, primero haga ``file > generate fonts``.

### Disminuir la cantidad de caracteres a unicamente ascii de 256.

-- ``encoding > reencode > custom``.

-- Seleccionar chars mas alla de 0xFF y elminarlas (``edit > clear``). Debe asegurarse que no quede nada, luego ``encoding > remove unused slots``. En ocasiones tambien es necesario hacer ``encoding > detach glymphs`` (con las seleccionadas unicamente).

-- Si no se eliminan los chars que se quieren eliminar, revisar nuevamente el encoding que se encuentre en "custom" puesto que con algunas acciones puede restaurarse uno diferente.

-- revise que los caracteres ascii esten en sus posiciones, 'a' en 0x61, etc.

-- Recomendacion: guardar el archivo en formato propio del fontforge (.sfd) como backup luego de haber reducido a 256 los chars.

-- asegurarse que todos los chars esten completos (aquellos que estan marcados con una cruz no seran generados en el bitmap font y genera un arreglo de menos de 256 caractere). Por ello, para rellenar todos los caracteres puede copiar el caracter "espacio" y pegarlo donde necesite.

-- Todos los chars tienen que tener encoding, por lo que seleccionar todas y hacer:
``encoding > force encoding > windows latin ansi``

-- Seleccionar "bitmap strikes" e ingresar el alto en pixeles deseado. Ejemplo: 16.

-- ``file > generate fonts``, y en el menu cambiar la extension del arhivo a guardar por .bdf

-- antes de guardar, seleccionar 16 como altura (o la que deba ser) todas las veces que deba indicarse el tamano.

-- va a generar un archivo con nombre xxxxx-16.bdf