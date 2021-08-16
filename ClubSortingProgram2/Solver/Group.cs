using System.Collections.Generic;

namespace ClubSortingProgram2.Solver
{
    public class Group
    {
        public readonly string Name;
        public List<Person> Members { get; private set; }

        private readonly int _maximumCapacity;

        public Group(string name, bool defaultGroup = false)
        {
            Name = name;
            _maximumCapacity = defaultGroup ? int.MaxValue : Solver.Settings.MaximumCapacity;
            Members = new List<Person>();
        }
    }
}