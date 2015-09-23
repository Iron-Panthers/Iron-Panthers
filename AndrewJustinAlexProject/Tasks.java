public class Tasks{
	private ArrayList<Task> tasks = new ArrayList<Task>();

	public String addTask(Task newTask){
		//check if the newTask is a dupe
		for (Task task in tasks){
			if (task.name.equals(newTask.name)){
				return "This is a duplicate."
			}
		}

		tasks.add(newTask);
		return "Your task named \""+newTask.name+"\" was successfully added."
	}

	public Task[] tasksForToday(){
		Task[] tasksToday = new Task[tasks.length()]
		int lastAdded = 0;
		for (Task task in tasksToday){
			if (task.daysUntilDue <= 1){
				tasksToday[i] = task;
				i++;
			}
		}
		return tasksToday;
	}

	public boolean allNextDay(){
		for (Task task in tasks){
			task.nextDay();
		}
	}

	public String finishTask(int taskToEnd){
		if (taskToEnd < tasks.length() && tasksToEnd >= 0){
			Task theTask = tasks.get(taskToEnd);
			if (theTask.daysUntilDue > 1){
				return "You finished this task "+(theTask.daysUntilDue-1)+" days early";
			} else if (theTask.daysUntilDue == 1){
				return "You finished this task on time.";
			} else {
				return "You finished this task "+(-1 * theTask.daysUntilDue +1)+" days late.";
			}
			tasks.remove(taskToEnd);
		} else {
			return "Index " + taskToEnd + " out of bounds.";
		}
	}
}
