/*

MenuItemBase* MenuLists:: addItem
  (
   const String&       command,
   const String&       cl_option_string,
   const String&       description_,
   const String&       default_answer,
   const String&       valid_answer,
         char          hot_key,
         char          cl_option_char,
         MenuCallBack* func
  )
{
  MenuItemBase* item = new MenuField(command,
                                     cl_option_string,
                                     description_,
                                     default_answer,
                                     valid_answer,
                                     hot_key,
                                     cl_option_char,
                                     func
                                     );
  bool ok;
  if (!itemlist.ok())  // first item put into list
    {
      itemlist.rebind(item);
      lastitem.rebind(item);
      ok = true;
    }
  else
    {
      ok = lastitem->addItem(item);
      lastitem.rebind(item);
    }
  if (!ok)
    errorFP("MenuLists::addItem",
            "could not add item, fundamental error or too little RAM");

  if (!virtual_first.ok())
    virtual_first.rebind(item);

#ifdef DP_DEBUG
  {
    s_o << "\nbool MenuLists:: addItem - MenuField";
    item->print4awk(s_o);
#ifdef WIN32
    s_o << endl;   // make debug output easier to read
#else
    s_o->flush();
#endif

  }
#endif
  return item;
}


void MenuLists::initCommandLine()
{
  if (cl_argv == NULL)
    fatalerrorFP("MenuLists::initCommandLine",
    "Command line arguments not available. You have forgotten to call\n"
    "initDiffpack from main!");

  input_source = STREAM; // default

  int i = 1;

  String cmd;
  while (i < cl_argc)
    {
      cmd = cl_argv[i];
      if (cmd == "+isg" || cmd == "--isg")
        input_source = GRAPHICAL;
      else if (cmd == "+iss" || cmd == "--iss")
        input_source = STREAM;
      else if (cmd == "--menufile" || cmd == "--prompt")
        input_source = STREAM;
      else if (cmd == "+isq" || cmd == "--isq")
        input_source = QUESTION_STREAM;
      else if (cmd == "+iscl" || cmd == "--iscl")
        input_source = COMMAND_LINE;
      else if (cmd == "--batch" || cmd == "--noprompt")
        input_source = COMMAND_LINE;
      else if (cmd == "+btgui" || cmd == "--btgui")
        input_source = BTGUI;
      else if (cmd == "+GUI" || cmd == "--GUI")
        input_source = BTGUI;

      // check typical typos:
      if (cmd == "-GUI")
        fatalerrorFP("MenuList::initCommandLine",
        "Found the reserved option -GUI, but you probably mean --GUI?");
      else if (cmd == "-iss")
        fatalerrorFP("MenuList::initCommandLine",
        "Found the reserved option -iss, but you probably mean --iss?");
      else if (cmd == "-iscl")
        fatalerrorFP("MenuList::initCommandLine",
        "Found the reserved option -iscl, but you probably mean --iscl?");
      else if (cmd == "-batch")
        fatalerrorFP("MenuList::initCommandLine",
        "Found the reserved option -batch, but you probably mean --batch?");
      else if (cmd == "+btmfc" || cmd == "--btmfc")
        fatalerrorFP("MenuList::initCommandLine",
        "The old option %s is no longer supported, use --GUI",cmd.c_str());


      i++;
  }
}



MenuItemBase* MenuLists:: getItemPtr
  (
   const String& in_command,
         char    inkey,
         bool sublevels
  )
{
  if (itemlist.ok())
    return itemlist->getItemPtr( in_command, inkey, sublevels);
  else
    return NULL;
}


MenuItemBase* MenuLists:: getItemPtr
  (
   const char* cl_arg,
   char cl_char
  )
{
  if (itemlist.ok())
    return itemlist->getItemPtr( cl_arg, cl_char);
  else
    return NULL;
}


String MenuLists:: get (const String& in_command, bool sublevels)
{
  static String str("");

  // if non-empty list: recursive search

  MenuItemBase* item = NULL;
  if (itemlist.ok())
    {
      // find the menu item corresponding to in_command (item):
      item = getItemPtr(in_command, '\0', sublevels);
      if (item)
        {
          // the item was found (in_command matches a menu item),
          str = item->get();
        }
      else
        fatalerrorFP("MenuLists::get",
        "Command \"%s\" was not found. Correct your call to MenuSystem's\n"
        "get(\"%s\").",in_command.c_str(),in_command.c_str());
    }
  return str;
}


void MenuLists:: set (const String& in_command, const String& value,
                      bool sublevels)
{
  MenuItemBase* item = NULL;
  if (itemlist.ok())
  {
    // find the menu item corresponding to in_command (item):
    item = getItemPtr(in_command, '\0', sublevels);
    if (item)
    {
      // the item was found (in_command matches a menu item),
      item->set(value);
    }
    else
      fatalerrorFP("MenuLists::set",
                   "Command \"%s\" was not found",in_command.c_str());
  }
}


void MenuLists:: search4multipleAnswers (MenuAnswerTable& table)
{
  if (virtual_first.ok())
    virtual_first->search4multipleAnswers (table);
  else
    errorFP("MenuLists::search4multipleAnswers",
            "You are using MenuSystem::multipleLoop and have erased the\
            \nmenu by calling MenuSystem::forget (f.ex. after scan). The\
            \nmenu is therefor empty now. You must remove your forget-call\
            \nto make the multiple answer functionality available.");
}


void MenuLists::print4awk (Os os)
{
  os << "\nMenuLists@" <<prompttext << " ";

  if (itemlist.ok())
    itemlist->print4awk (os);   // recursive search
  os << "\n";
  os->flush();
}



void MenuLists::prompt (Is in)
{
#ifdef DP_DEBUG
  {
    s_o << "\n ---------------------------------------------------";
    s_o << "\nvoid MenuLists::prompt(Is in) - starting up mode: "
        << input_source;
    s_o << "  MENULEVEL = " << MENULEVEL;
    s_o->flush();
  }
#endif

  if (lastitem.ok())
    lastitem->last_before_prompt = true;

  switch (input_source)
    {
    case STREAM:
      promptStream (in, true);
      break;
    case QUESTION_STREAM:
      promptQuestion();
      break;
    case COMMAND_LINE:
      promptCommandLine();
      break;
    case GRAPHICAL:
      errorFP("MenuLists::prompt",
      "The command line argument +isg is no longer supported.\n\n"
      "Instead, compile your application Make GUIMENU=tcl. This\n"
      "option can be set as deafult in the $BTR/src/MakeFlags file\n"
      "or in your local.cmake2 file. To run the final application\n"
      "with a graphical user interface, use the command line option --GUI.");

      break;
    default:
      errorFP("Menu:prompt",
              "This type of input source ($d) is not implemented",
              input_source);
    }
}


void MenuLists::promptStream(Is in, bool interactive)
{
  // If an input file is given with the -D commandline option,
  // the command/value pairs from the input file are here
  // loaded into objects in the global menu.

  bool finished = false;
  String in_command("");
  String word;
  String value;
  char eqmark = '=';
  int len;
  char hot_key;
  bool cin_stream = check_Is_cin(in);  // is Is really the istream cin?

  Handle(MenuItemBase) itemptr;

  while (!finished)
  {
    hot_key = '\0';
    if (cin_stream)
    {
      s_o << "\n"<<prompttext<<" > ";
      s_o->flush();
    }

    in->getline(in_command,'\n');  // this is the only Is-command here
    in_command.strip();

//DBP(oform("promptStream: got the line [%s]",in_command.c_str()))

    if (in_command.contains(' '))  // extract the first word
    {
      word = in_command.before(' ');  // word is a new copy
      word.strip();
    }
    else  // the whole in_command is one word (no blanks)
      word = in_command.pureCopy();
    in_command.del(word);
    if (in_command.contains('!'))    // remove comments from input !
      in_command = in_command.before('!');
    in_command.strip();

    len = word.size();
    if (len == 0 || !word.ok())
    {
      word = "\n";        // the user gave (blanks and) carriage return
      len = 1;
    }
    if (len == 1)         // the first word of the command was a hotkey
      hot_key = word[0];

    if ((logfileSet ) &&
        !((hot_key == 'l') || (strncmp(word.c_str(),"log",len)==0)))
      logfile << "\n"<< word << " " << in_command;

      // check which command  word  was and perform action:
    if (word[0] == '\n')  // not a command, user just gave carriage return
        ;
    else if ((hot_key == '?') || (strncmp(word.c_str(),"help",len)==0))
        help(in_command);
    else if ((hot_key == 'f') || (strncmp(word.c_str(),"file",len)==0))
        file(in_command.c_str());
    else if ((hot_key == 'q') || (strncmp(word.c_str(),"quit",len)==0))
        quit(in);
    else if ((hot_key == 'c') || (strncmp(word.c_str(),"continue",len)==0))
    {
      if (interactive)
        finished = true;
    }
    else if ((hot_key == 'x') || (strncmp(word.c_str(),"ok",len)==0))
    {
      finished = true;
    }
    else if ((hot_key == 'l') || (strncmp(word.c_str(),"log",len)==0))
      log (in_command.c_str());

    else if (word == "set") // MenuField command read
    {
      if (in_command.contains(eqmark))
      {
        // let word become the answer:
        value = in_command.after(eqmark);
        value.strip();
        in_command = in_command.before(eqmark);  // (takes a copy!)
      }
      else  // no = character, answer is missing:
      {
        value = "";
        warningFP("MenuLists::promptStream",
            "Expected '%c' in the command, cannot find answer",
            eqmark);
      }

      // process command (in_command is now the user's command)
      in_command.strip();

//DBP(oform("Menu: word=[%s], value=[%s] and in_command=[%s]\n",word.c_str(),value.c_str(),in_command.c_str()))
      if (in_command.size() == 1)
        hot_key = in_command[0];
      else
        hot_key = '\0';

      if (virtual_first.ok())      // find MenuItemBase for command;
      {
        itemptr.rebind(
                  virtual_first->getItemPtr(in_command, hot_key, false));

        if (itemptr.ok())
        {
          // command was found, set answer and issue callback:
          itemptr->setAnswer(value);
          itemptr->issueCallback ();
        }
        else if (cin_stream && interactive)
          warningFP("MenuLists::promptStream",
          "Command \"%s\" (given as input to the program) was not found.\n"
          "Type help to see the legal commands", in_command.c_str());
      }
      else
        errorFP("promptStream",
                "No items in the menu tree (no items at all or no new\
                \nitems since last call to forget())");
    }

    else if (word == "sub") // MenuSub command read
    {
      if (in_command.empty())
      {
        warningFP("MenuLists::promptStream",
                "You wrote \"sub\", but where is the submenu name?");
        break;
      }
      if (in_command.size() == 1)
        hot_key = in_command[0];
      else
        hot_key = '\0';

      if (virtual_first.ok())      // find MenuItemBase for command;
      {
        itemptr.rebind(
                  virtual_first->getItemPtr(in_command, hot_key, false));
        if (itemptr.ok())
          itemptr->prompt(in);
        else if (cin_stream && interactive)
          errorFP("MenuLists::promptStream",
                  "submenu \"%s\" was not found",in_command.c_str());
      }
      else
        errorFP("promptStream",
                "The menu is empty (no items at all or no new items\
                \ndefined since last call to forget())");
    }
    if (logfileSet)
      logfile->flush();
  }
  if (cin_stream)
  {
    s_o << "\n\n"; s_o->flush();
  }
}


void MenuLists::promptQuestion()
{
  if (virtual_first.ok())
    virtual_first->promptQuestion();
}



void MenuLists::promptCommandLine()
{
  int i = 1;
  char ch;
  String value;
  String cmd, cmd_stripped;
  Handle(MenuItemBase) item;

  // first check -D or --Default to read in default values
  while (i < cl_argc)
    {
      cmd = cl_argv[i];

      //DBP(oform("promptCmdL 1: command=[%s]",cmd.c_str()))

      if (( cmd == "-D") || (cmd == "+Default") || (cmd == "--Default"))
        {
          if (i+1 >= cl_argc)
            errorFP("MenuLists::promptCommandLine",
                    "Missing argument (filename) to %s command",cmd.c_str());

          Is defaultfile (cl_argv[++i], INFILE);
          promptStream (defaultfile, false);
        }
      // this one is likely not to be supported in the future:
      else if (cmd == "+logfile" || cmd == "--logfile")
        {
          if (i+1 >= cl_argc)
            fatalerrorFP("MenuLists::promptCommandLine",
            "Missing argument (filename) to %s command",cmd.c_str());
          file(cl_argv[++i]);
        }
      i++;
    }

  // then process menu commands:

  i = 1;
  bool possible_menu_command, possible_error;
  while (i < cl_argc)
    {
      cmd = cl_argv[i];
      ch = '\0';
      possible_menu_command = false;
      possible_error = false;

      //DBP(oform("promptCmdL 2: command=[%s]",cmd.c_str()))

      if (cmd.size() == 2 && cmd[0] == '-') {    // short command
        cmd_stripped = String(cmd[1]);
        ch = cmd[1];
        possible_menu_command = true;
      }
      else if (cmd[0] == '-' && cmd[1] == '-') {// command starting with --
        cmd_stripped = cl_argv[i]+2;
        possible_menu_command = true;
      }
      else if (cmd[0] == '+') {                 // for backward compatibility
        cmd_stripped = cl_argv[i]+1;
        possible_menu_command = true;
      }
      else if (cmd[0] == '-' && cmd[1] != '-') {
        cmd_stripped = cl_argv[i]+1;
        possible_error = true;
        possible_menu_command = true;
      }
      if (possible_menu_command) {
        item.rebind(getItemPtr (cmd_stripped.c_str(), ch));
        //s_o<<"Searching for i="<<i<<" :"<<cl_argv[i]<<" ["<<cmd_stripped<<"] and found="<<item.ok()<<'\n';
        if (item.ok() && possible_error) {
          // the command started with -, must start with --
          fatalerrorFP("MenuLists::promptCommandLine",
                       "You gave the command "
                       "line option \"-%s\", but according to the menu,\n"
                       "it should read \"--%s\".",
                       cmd_stripped.c_str(),cmd_stripped.c_str());
        }
      }
      else
        item.detach();


      if (!item.ok()) {
        // ignore warning here, --casename and --GUI are f.ex. not
        // among the commands defined in the menu and they must be
        // silently processed...
      }
      else if (i+1 < cl_argc)
        {
          value = cl_argv[++i];
          //s_o<<"Just assigned value=["<<value<<"]\n";
          item->setAnswer(value);
        }
      else
        fatalerrorFP("MenuLists::promptCommandLine",
                     "Missing next argument of command line arg. \"%s\"",
                     cl_argv[i]);
      i++;
    }
}

void MenuLists::help (const String& input)
{
  if (input.empty())
  {
    s_o << oform("\n h%9s"," ") << oform("%-9s","- help");
    s_o << "[list all menu items and their answers] ";
    s_o << oform("\n x%9s"," ") << oform("%-9s","- ok");
    s_o << "[quit this menu]";
    s_o << oform("\n h command %-9s","- help cmd");
    s_o << "[details of a specific command]";
    s_o << oform("\n q%9s"," ")  << oform("%-9s","- quit");
    s_o << "[exit program]";
    s_o->flush();
    printCommandList(s_o, false);
  }
  else
  {
    char hotkey;
    Handle(MenuItemBase) item;
    if (input.size() == 1)
      hotkey = input.firstchar();
    else
      hotkey = '\0';
    if (virtual_first.ok())      // find MenuItemBase for command;
    {
      item.rebind(virtual_first->getItemPtr(input, hotkey, false));
      if (item.ok())
        item->helpCommand(s_o);
      else
        warningFP("MenuLists::help",
                  "Command \"%s\" was not found",
                  input.c_str());
    }
  }
}

void MenuLists::file (const char* filename)
{
  if (filename[0] == '\0' || filename == NULL)
   {
      warningFP("MenuLists::file","no inputfile given");
      return;
    }

  InputSource old_input_source = input_source; // enum variables
  input_source = STREAM;  // unnecessary, promptStream should work anyway

  Is infile(filename,INFILE);
  promptStream(infile,false);
  input_source = old_input_source;  // ?????
}


void MenuLists:: log (const String& filename)
{
  String f = filename;
  logfileSet = !logfileSet;
  if (logfileSet)
  {
    if ( filename.empty())// == NULL )
      f = "logfile.txt";
    logfile.rebind(f.c_str(),NEWFILE);
    s_o << "\n write all commands to logfile: \""<< f <<"\"";
  }
  else
  {
    s_o << "\n logfile closed. ";s_o->flush();
    logfile << "\nok\n";
    logfile->flush();
    logfile->close();
  }
  s_o->flush();
}

*/


