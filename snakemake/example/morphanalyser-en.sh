apertium-destxt | lt-proc /home/lpla/Downloads/eng.bin | apertium-retxt | sed 's:\^[^/]\+/\*\?::g' | sed 's:\([⁰¹²³⁴⁵⁶⁷⁸⁹]\)*\(<[^\$]\+\)*\$::g'
