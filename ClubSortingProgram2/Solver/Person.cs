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
                MainScreen.Instance.AlertError(week.ToString() + "out of bounds; there are only " + Requests.Length + "weeks.");
                return;
            }
            if (requests.Count > Solver.Settings.MaxRequests)
            {
                MainScreen.Instance.AlertError(Name + " has too many requests: " + requests.Count + ".");
                return;
            }

            if (Requests[week].Count != 0) Requests[week].Clear();
            foreach (Group c in requests) Requests[week].Add(c);
        }
    }
}