using Newtonsoft.Json.Serialization;
using Newtonsoft.Json;
using System;

namespace WPFChat
{
    [JsonObject(MemberSerialization.OptIn)]
    public class ChatEntry
    {
        [JsonProperty(PropertyName = "id")]
        public int ID { get; set; }

        [JsonProperty(PropertyName = "login")]
        public string Login { get; set; }

        [JsonProperty(PropertyName = "cnt")]
        public string Text { get; set; }

        [JsonProperty(PropertyName = "dts")]
        public string Time { get; set; }

        public override bool Equals(object obj)
        {

            if (!(obj is ChatEntry)) return false;

            return this == (ChatEntry)obj;

        }
        public static bool operator ==(ChatEntry a, ChatEntry b)
        {
            return a.ID == b.ID;
        }
        public static bool operator !=(ChatEntry a, ChatEntry b)
        {
            return a.ID != b.ID;
        }
        public static bool operator >(ChatEntry a, ChatEntry b)
        {
            return Convert.ToInt64(a.ID) > Convert.ToInt64(b.ID);
        }
        public static bool operator <(ChatEntry a, ChatEntry b)
        {
            return Convert.ToInt64(a.ID) < Convert.ToInt64(b.ID);
        }
        public static int Comparison(ChatEntry a, ChatEntry b)
        {

            if (Convert.ToInt64(a.ID) > Convert.ToInt64(b.ID))

                return -1;

            else if (Convert.ToInt64(a.ID) == Convert.ToInt64(b.ID))

                return 0;

            else if (Convert.ToInt64(a.ID) < Convert.ToInt64(b.ID))

                return 1;

            return 0;

        }
    }
}
