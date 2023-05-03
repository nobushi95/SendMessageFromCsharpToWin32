using PInvoke;

namespace MessageSenderConsoleCsharp
{
    internal class Program
    {
        private static readonly string ListenerWindowClassName = "MessageListenerWin32_WindowClassName";
        private static readonly IntPtr ListenerWindowHandle = User32.FindWindow(ListenerWindowClassName, null);
        private static readonly User32.WindowMessage CustomWindowMessage1 = User32.WindowMessage.WM_APP + 0;
        private static readonly User32.WindowMessage CustomWindowMessage2 = User32.WindowMessage.WM_APP + 1;

        static void Main(string[] args)
        {
            Console.WriteLine("--- Message Sender Console Csharp ---");
            while (true)
            {
                Console.WriteLine("1: WM_CUSTOM1");
                Console.WriteLine("2: WM_CUSTOM2");
                Console.Write("> ");
                var num = Console.ReadLine();
                switch (num)
                {
                    case "1":
                    {
                        User32.PostMessage(ListenerWindowHandle, CustomWindowMessage1, new IntPtr(10), new IntPtr(11));
                        break;
                    }
                    case "2":
                    {
                        User32.PostMessage(ListenerWindowHandle, CustomWindowMessage2, new IntPtr(20), new IntPtr(21));
                        break;
                    }
                    case "exit":
                    {
                        return;
                    }
                    default:
                    {
                        Console.WriteLine("Input must be \"1\" or \"2\".");
                        Console.WriteLine("Input \"exit\" finish program.");
                        break;
                    }
                }
                Console.WriteLine("");
            }
        }
    }
}
