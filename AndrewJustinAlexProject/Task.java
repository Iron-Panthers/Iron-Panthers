import java.lang.String;

public class Task
{
	public String name;
	public String description;
	public int priority;
	public int daysUntilDue;
	
	public Task (String taskName, String taskDescription, int taskPriority, int taskDaysDue){
		name=taskName;
		description=taskDescription;
		priority=taskPriority;
		daysUntilDue=taskDaysDue;
	}
	
	public void nextDay(){
		daysUntilDue--;
	}
}
