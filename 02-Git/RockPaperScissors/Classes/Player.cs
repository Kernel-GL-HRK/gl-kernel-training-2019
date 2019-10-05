namespace RockPaperScissors.Classes
{
    public class Player
    {
        private string strategy;

        public Player(string[] values)
        {
            this.Name = values[0];
            this.Strategy = values[1];
        }

        public string Name { get; set; }

        public string Strategy
        {
            get { return strategy; }
            set { strategy = value.ToUpperInvariant(); }
        }
    }
}
