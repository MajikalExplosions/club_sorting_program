using System.Collections.Generic;

namespace ClubSortingProgram2.Solver
{
    public class Group
    {
        public readonly string Name;
        public List<Person> Members { get; private set; }

        public readonly int MaximumCapacity;

        public Group(string name, bool defaultGroup = false)
        {
            Name = name;
            MaximumCapacity = defaultGroup ? int.MaxValue : Solver.Settings.MaximumCapacity;
            Members = new List<Person>();
        }
    }
}