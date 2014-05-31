using System.Windows;
using System.Net;
using System.IO;
using System.Collections.Generic;
using Newtonsoft.Json;
using System.Collections.ObjectModel;
using System.Windows.Input;

namespace WPFChat
{
    public partial class Window1 : Window
    {
        private List<ChatEntry> allEntries=null;
        public Window1()
        {
            InitializeComponent();
            allEntries = new List<ChatEntry>();
            sent output = new sent();
            this.DataContext = output;
            msg.KeyDown += keydown;
            button1.Click += button1_Click;
            output.User = "katjes";
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(@"http://www.stud.fit.vutbr.cz/~xmlich02/itu-ajax/api.php");

            // Blokující čekání na odpověď
            HttpWebResponse response = (HttpWebResponse)request.GetResponse();

            // Přečteme odpověď a uložíme do řetězce
            StreamReader reader = new StreamReader(response.GetResponseStream());
            string responseText = reader.ReadToEnd();
            List<ChatEntry> entries = JsonConvert.DeserializeObject<List<ChatEntry>>(responseText);
            entries.Reverse();
            allEntries.AddRange(entries);
            listbox_franta_kecup.ItemsSource = allEntries;
        }

        private void send()
        {
            if (!string.IsNullOrEmpty(msg.Text) && !string.IsNullOrEmpty(user.Text))
            {
                string url = string.Format("http://www.stud.fit.vutbr.cz/~xmlich02/itu-ajax/api.php?data={0}&user={1}", msg.Text, user.Text);
                HttpWebRequest request = (HttpWebRequest)WebRequest.Create(url);

                // Blokující čekání na odpověď
                HttpWebResponse response = (HttpWebResponse)request.GetResponse();
                msg.Text = "";
                // Přečteme odpověď a uložíme do řetězce
                StreamReader reader = new StreamReader(response.GetResponseStream());
                string responseText = reader.ReadToEnd();
            }
            HttpWebRequest brequest = (HttpWebRequest)WebRequest.Create(@"http://www.stud.fit.vutbr.cz/~xmlich02/itu-ajax/api.php");

            // Blokující čekání na odpověď
            HttpWebResponse bresponse = (HttpWebResponse)brequest.GetResponse();

            // Přečteme odpověď a uložíme do řetězce
            StreamReader breader = new StreamReader(bresponse.GetResponseStream());
            string bresponseText = breader.ReadToEnd();

            List<ChatEntry> entries = JsonConvert.DeserializeObject<List<ChatEntry>>(bresponseText);
            allEntries.AddRange(entries.FindAll(p => !allEntries.Contains(p)));
            allEntries.Sort(ChatEntry.Comparison);
            listbox_franta_kecup.ItemsSource = null;
            listbox_franta_kecup.ItemsSource = allEntries;
        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {
            send();
        }

        private void keydown (object sender, KeyEventArgs n)
        {
            if (n.Key == Key.Enter)
            {
                n.Handled = true;
                send();
            }
        }
    }

    public class sent
    {
        public string User { get; set; }
        public string Mesg { get; set; }
    }

    public class show
    {
        public ObservableCollection<ChatEntry> mess { get; set; }

        public show(List<ChatEntry> entries)
        {
            mess = new ObservableCollection<ChatEntry>(entries);
        }
    }
}
