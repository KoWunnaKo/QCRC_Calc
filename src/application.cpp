#include "application.h"

#ifdef Q_OS_WIN
    #include <windows.h>  //need for support console (It's Windows baby)
#endif

#include <iostream>
#include <getopt.h>



#define STRINGIFY(x) #x
#define DEF_TO_STR(x) STRINGIFY(x)





Application::Application(int &argc, char **argv) :
    QGuiApplication(argc, argv)
{
    setApplicationDisplayName("CRC Calculator ver "
                              DEF_TO_STR(MAJOR_VERSION) "."
                              DEF_TO_STR(MINOR_VERSION));

    attach_console();
    processing_cmd(argc, argv);


    calc_text.set_ucrc(&uCRC);
    calc_hex.set_ucrc(&uCRC);


    // GUI
    engine.rootContext()->setContextProperty("uCRC", &uCRC);
    engine.rootContext()->setContextProperty("calc_text", &calc_text);
    engine.rootContext()->setContextProperty("calc_hex", &calc_hex);

    engine.load(QUrl(QLatin1String("qrc:/qml/main.qml")));
}



void Application::attach_console()
{

#ifdef Q_OS_WIN

    // attach the new console to this application's process
    AttachConsole(ATTACH_PARENT_PROCESS);

   // reopen the std I/O streams to redirect I/O to the new console
   freopen("CON", "w", stdout);
   freopen("CON", "w", stderr);
   freopen("CON", "r", stdin);

#endif

}



static const char *help_str =
        " ===============  Help  ===============\n"
        " Application:  CRC Calculator\n"
        " App version:  "  DEF_TO_STR(MAJOR_VERSION) "." DEF_TO_STR(MINOR_VERSION) "\n"
#ifdef  QT_DEBUG
        " Build  mode:  debug\n"
#else
        " Build  mode:  release\n"
#endif
        " Build  date:  " __DATE__ "\n"
        " Build  time:  " __TIME__ "\n\n"
        "Options:                      description:\n\n"
        "       --bits         [value] Set Bits    (value 1..64)\n"
        "       --poly         [value] Set Polynom (value in hex)\n"
        "       --init         [value] Set Init    (value in hex)\n"
        "       --xor_out      [value] Set XorOut  (value in hex)\n"
        "       --ref_in       [value] Set RefIn   (value in hex)\n"
        "  -v   --version              Display version information\n"
        "  -h,  --help                 Display this information\n\n";



// indexes for long_opt function
namespace LongOpts
{
    enum
    {
        version = 0,
        help,

        // Set CRC param
        bits,
        poly,
        init,
        xor_out,
        ref_in
    };
}



static const struct option long_opts[] =
{

    { "version",      no_argument,       NULL, LongOpts::version       },
    { "help",         no_argument,       NULL, LongOpts::help          },

    // Set CRC param
    { "bits",         required_argument, NULL, LongOpts::bits          },
    { "poly",         required_argument, NULL, LongOpts::poly          },
    { "init",         required_argument, NULL, LongOpts::init          },
    { "xor_out",      required_argument, NULL, LongOpts::xor_out       },
    { "ref_in",       required_argument, NULL, LongOpts::ref_in        },

    { NULL,           no_argument,       NULL,  0                      }
};



void Application::processing_cmd(int argc, char *argv[])
{
    int opt;

    while( (opt = getopt_long(argc, argv, "", long_opts, NULL)) != -1 )
    {

        switch( opt )
        {

            case LongOpts::version:
                    std::cout << "CRC Calculator version "
                              << MAJOR_VERSION << '.' << MINOR_VERSION << "\n";
                    _exit(EXIT_SUCCESS);
                    break;


            case LongOpts::help:
                    std::cout << help_str;
                    _exit(EXIT_SUCCESS);
                    break;


            // Set CRC param
            case LongOpts::bits:
                    if( uCRC.set_bits(str_to_uint64(optarg)) != 0 )
                    {
                        std::cout << "Cant set bits from val: " << optarg << "\n";
                        _exit(EXIT_FAILURE);
                    }
                    break;


            case LongOpts::poly:
                    uCRC.set_poly(str_to_uint64(optarg, 16));
                    break;


            case LongOpts::init:
                    uCRC.set_init(str_to_uint64(optarg, 16));
                    break;


            case LongOpts::xor_out:
                    uCRC.set_xor_out(str_to_uint64(optarg, 16));
                    break;


            case LongOpts::ref_in:
                    uCRC.set_ref_in(str_to_bool(optarg));
                    break;


            default:
                    // getopt_long function itself prints an error message
                    std::cout << "for more detail see help\n\n";
                    _exit(EXIT_FAILURE);
                    break;
        }
    }
}



quint64 Application::str_to_uint64(const char *val, int base)
{
    bool ok;
    QString tmp(val);

    quint64 res = tmp.toULongLong(&ok, base);

    if(!ok)
    {
        std::cout << "Cant convert: " << val << " to hex\n";
        _exit(EXIT_FAILURE);
    }


    return res; //good job
}



bool Application::str_to_bool(const char *val)
{
    QString tmp(val);

    return (tmp.compare("true", Qt::CaseInsensitive) == 0) || (atoi(val) != 0);
}
