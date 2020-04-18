.lac File Structure
===================


.. csv-table:: Basic file structure
   :header: "Name", "From", "To", "Size", "Description"
   :widths: 10,10,10,10,40

   "program_start", "0","3","4","Where to start loading memory (program start)"
   "meta", "4", "program_start.value-1", "program_start.value-4", "Metadata. How to use TBD."
   "bytecode", "program_start.value", "FILE_END", "FILE_SIZE-program_start", "La bytecode"

Metadata structure
=============================

TBD.
