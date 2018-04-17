- Certain command line options are not handled rightly. For example,
  "./sampleRoseTranslator -rose:verbose test.C"
  causes a seg fault.I figured it may due to 
  "CommandlineProcessing::isOptionWithParameter". It seems no check
  whether there is a real parameter provided for an option.
  I see this because I use CommandlineProcessing::
  isOptionWithParameter(argc, argv, "--uc:", 
  "polymorphic-unit", uc_polymorphic_unit, true); in my code, and
  "./UC test.C --uc:polymorphic-unit" seg faults but "./UC test.C 
   --uc:polymorphic-unit 1" works fine.

Best regards,

Lingxiao Jiang

