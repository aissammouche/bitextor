apertium-destxt | lt-proc /home/lpla/Downloads/fra.bin | apertium-retxt | sed 's:\^[^/]\+/\*\?::g' | sed 's:\([⁰¹²³⁴⁵⁶⁷⁸⁹]\)*\(<[^\$]\+\)*\$::g'
