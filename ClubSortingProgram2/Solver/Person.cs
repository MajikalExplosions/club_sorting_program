using System.Collections.Generic;

namespace ClubSortingProgram2.Solver
{
    public class Person
    {
        public readonly string Name;
        public Group[] Assignments { get; private set; }
        public List<Group>[] Requests { get; private set; }

        public Person(string name)
        {
            Name = name;
            Assignments = new Group[Solver.Settings.Sections];
            Requests = new List<Group>[Solver.Settings.Sections];
        }

        public void SetRequests(int week, List<Group> requests)
        {
            if (week < 0 || week > Requests.Length - 1)
            {
                throw new SolverException(100, string.Format("{0} out of bounds; there are only {1} section(s).", week, Requests.Length));
            }
            if (requests.Count > Solver.Settings.MaxRequests)
            {
                throw new SolverException(101, string.Format("{0} has too many requests ({1}, max. {2})", Name, requests.Count, Solver.Settings.MaxRequests));
            }
            if (Requests[week].Count != 0) Requests[week].Clear();
            foreach (Group c in requests)
            {
                if (!Requests[week].Contains(c) && !c.Name.Equals(Solver.Settings.DefaultGroupName)) Requests[week].Add(c);
            }
        }
    }
}