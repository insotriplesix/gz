package other;

/** This class stores all player's attributes for pull them into the next level. **/
public class AttributesStorage {
	/* Player attributes */
	private static int health, maxHealth, stamina, maxStamina,
		damage, chargeDamage, chargingTick, score;

	/* HUD attributes */
	private static long time = 0;
	
	/* Initialize default attributes values */
	public static void init() {
		health = maxHealth = 100;
		stamina = maxStamina = 150;
		damage = 1;
		chargeDamage = 2;
		score = 0;
		time = 0;
	}
	
	/* Get each of the attributes */
	public static int getHealth() { return health; }
	public static int getMaxHealth() { return maxHealth; }
	public static int getStamina() { return stamina; }
	public static int getMaxStamina() { return maxStamina; }
	public static int getDamage() { return damage; }
	public static int getChargeDamage() { return chargeDamage; }
	public static int getCharginTick() { return chargingTick; }
	public static int getScore() { return score; }
	
	public static long getTime() { return time; }

	/* Set the value for each of the attributes */
	public static void setHealth(int h) { health = h; }
	public static void setMaxHealth(int mh) { maxHealth = mh; }
	public static void setStamina(int s) { stamina = s; }
	public static void setMaxStamina(int ms) { maxStamina = ms; }
	public static void setDamage(int d) { damage = d; }
	public static void setChargeDamage(int cd) { chargeDamage = cd; }
	public static void setCharginTick(int ct) { chargingTick = ct; }
	public static void setScore(int sc) { score = sc; }
	
	public static void setTime(long t) { time = t; }
	
}
