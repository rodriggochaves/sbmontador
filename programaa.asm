;Codigo fonte do módulo A

section text
mod_a:  begin
y:      extern
mod_b:  extern
        public   val
        public   l1
        input    y
        load     val
        add      y
        store    y+2
        jmpp    mod_b
l1:     stop

section data
val:    const    5
        end