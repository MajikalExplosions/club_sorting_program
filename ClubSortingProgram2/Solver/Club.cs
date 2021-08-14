using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClubSortingProgram2.Solver
{
    public class Club
    {
        public readonly string Name;
        public List<Student> Assignments { get; private set; }

        private readonly int _maximumCapacity;

        public Club(string name, bool defaultClub = false)
        {
            Name = name;
            _maximumCapacity = defaultClub ? int.MaxValue : Solver.Settings.MaximumCapacity;
            Assignments = new List<Student>();
        }
    }
}
