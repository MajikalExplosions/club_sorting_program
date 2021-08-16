using Google.OrTools.Graph;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClubSortingProgram2.Solver
{
    public class Solver
    {
        public static readonly SolverSettings Settings;
        private readonly SortedList<string, Group> _groups;
        private readonly SortedList<string, Person> _people;

        private readonly Dictionary<string, int[]> _assignmentArcMap;
        private readonly Dictionary<string, int> _peopleNodeMap;
        private readonly Dictionary<string, int> _groupNodeMap;

        private bool _solved;

        public Solver()
        {
            _groups = new SortedList<string, Group>();
            _people = new SortedList<string, Person>();

            _groups.Add(Settings.DefaultGroupName, new Group(Settings.DefaultGroupName, true));
        }

        public void AddPerson(Person s)
        {
            if (_solved)
            {
                MainScreen.Instance.AlertError("Could not add person \"" + s.Name + "\" because assignments have been created.");
                return;
            }
            try
            {
                _people.Add(s.Name, s);
            }
            catch (ArgumentException)
            {
                MainScreen.Instance.AlertError("Found duplicate name: \"" + s.Name + "\".");
            }
        }

        public void AddGroup(Group c)
        {
            if (_solved)
            {
                MainScreen.Instance.AlertError("Could not add club \"" + c.Name + "\" because assignments have been created.");
                return;
            }
            try
            {
                _groups.Add(c.Name, c);
            }
            catch (ArgumentException)
            {
                MainScreen.Instance.AlertError("Found duplicate group name: \"" + c.Name + "\".");
            }
        }

        public void Solve()
        {
            if (_solved) return;
            //TODO Finish this section (see notes)

            MinCostFlow minCostFlow = new MinCostFlow();
            //TODO Create supply/sink nodes
            //TODO Assign node numbers to students and groups and fill in maps
            _createAssignmentArcs();
            //TODO Set supply and sink
            MinCostFlowBase.Status status = minCostFlow.SolveMaxFlowWithMinCost();
            if (status != MinCostFlowBase.Status.OPTIMAL)
            {
                MainScreen.Instance.AlertError("Could not find optimal solution.");
                return;
            }

            //TODO Find answers and update objects
            _solved = true;
        }

        public Group[] GetGroups(string name)
        {
            if (!_solved)
            {
                MainScreen.Instance.AlertError("Group assignments have not been created yet.");
                return null;
            }
            if (!_people.ContainsKey(name))
            {
                MainScreen.Instance.AlertError("Person \"" + name + "\" does not exist.");
                return null;
            }
            return _people[name].Assignments;
        }

        public Person[] GetMembers(string name)
        {
            if (!_solved)
            {
                MainScreen.Instance.AlertError("Group assignments have not been created yet.");
                return null;
            }
            if (!_groups.ContainsKey(name))
            {
                MainScreen.Instance.AlertError("Group \"" + name + "\" does not exist.");
                return null;
            }
            return _groups[name].Members.ToArray();
        }

        private void _createAssignmentArcs()
        {
            //TODO Create arcs for assignments
            //TODO Fill in arc map with data so we can fetch results.
        }

        public struct SolverSettings
        {
            public readonly int MaximumCapacity;
            public readonly int Sections;
            public readonly string DefaultGroupName;
            public readonly bool RequestsHaveOrder;
            public readonly int MaxRequests;

            public SolverSettings(int mc, int s, string dgn, bool rho, int mr)
            {
                MaximumCapacity = mc;
                Sections = s;
                DefaultGroupName = dgn;
                RequestsHaveOrder = rho;
                MaxRequests = mr;
            }
        }
    }
}